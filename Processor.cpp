//
// Created by jwc on 10/19/18.

#include <fstream>
#include <iomanip>
#include <dirent.h>
#include <string>
#include <sys/stat.h>
#include "Processor.h"
#include "Action.h"
#include "file_exists.h"
#include <linux/limits.h>

Processor::Processor(std::string filename) {
    std::cout << "Processor::Processor()" << std::endl;

    config = YAMLtoJSON(YAML::LoadFile(filename));

    metaDir = config.at("paths").at("meta dir");
    migrationsDir = config.at("paths").at("migrations dir");
    migrationFile = config.at("paths").at("migrations file");

    actions = config.at("actions");
    map = config.at("map");

    if (map.is_string()) {
        map = YAMLtoJSON(YAML::LoadFile(map));
    }
    if (!map.is_object()) {
        throw MergeException("Invalid type for map in config.yaml");
    }

    try {
        migrationData = read(migrationFile);
    }
    catch (MergeException *e) {
        migrationData = config.at("migration data");
        write(migrationFile, migrationData);
    }

    int currentSerial = getCurrentSerial();
    if (currentSerial != -1) {
        it = findMigration(currentSerial);
    }
}

void Processor::scanMigrations() {
    std::cout << "Processor::scanMigrations()" << std::endl;

    migrationData["migrations"] = json::array();
    scanMigrations(migrationsDir);
    write(migrationFile, migrationData);

    int currentSerial = getCurrentSerial();
    if (currentSerial != -1)
        it = findMigration(currentSerial);
}

int Processor::getCurrentSerial() {
    std::cout << "Processor::getCurrentSerial()" << std::endl;
    nlohmann::json migrationCurrent = migrationData.at("current");

    if (migrationCurrent.is_null()) {
        return -1;
    }

    int currentSerial = migrationCurrent;
    it = findMigration(currentSerial);

    return (*it)["serial"];
}

void Processor::setCurrentSerial(int s) {
    std::cout << "Processor::setCurrentSerial()" << std::endl;
    migrationData["current"] = s;
    write(migrationFile, migrationData);
}

nlohmann::json::iterator Processor::findMigration(int targetSerial) {
    std::cout << "Processor::findMigration()" << std::endl;

    nlohmann::json::iterator migrationIt;
    nlohmann::json *migrations = &migrationData.at("migrations");

    for (migrationIt = (*migrations).begin(); migrationIt != (*migrations).end(); ++migrationIt) {
        if ((*migrationIt).is_null()) {
            continue;
        }
        int serial = (*migrationIt).at("serial");
        if (targetSerial == serial) {
            return migrationIt;
        }
    }
    throw new MergeException("Migration not found.");
}

void Processor::scanMigrations(std::string &md) {
    //std::cout << "Processor::scanMigrations(): " << md << std::endl;

    DIR *dir;
    struct dirent *dirent;
    struct stat fileInfo;

    if ((dir = opendir(md.c_str())) == NULL) {
        throw new MergeException("Could not open directory.");
    }

    while ((dirent = readdir(dir)) != NULL) {
        std::string s = md + "/" + dirent->d_name;

        if (strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..") == 0) {
            continue;
        }

        stat(s.c_str(), &fileInfo);
        if (S_ISDIR(fileInfo.st_mode)) {
            scanMigrations(s);
        } else if (!S_ISREG(fileInfo.st_mode)) {
            throw new MergeException("Unknown file type");
        }

        if (strlen(dirent->d_name) > 5 && !strcmp(dirent->d_name + strlen(dirent->d_name) - 5, ".yaml")) {
            YAML::Node migration = YAML::LoadFile(s);

            if (!migration.IsMap()) {
                std::cout << "Migration not a map, skipping: " << s << std::endl;
                continue;
            }

            YAML::Node serialNode = migration["serial"];
            if (!serialNode.IsScalar()) {
                std::cout << "Serial not IsScalar, skipping: " << s << std::endl;
                continue;
            }
            int serial = serialNode.as<int>();

            YAML::Node upNode = migration["up"];

            if (upNode.size() > 0) {
                if (!upNode.IsSequence()) {
                    throw new MergeException("Invalid UP YAML::Node type");
                }
                setMigration(serial, s, "up", upNode);
            }

            YAML::Node downNode = migration["down"];

            if (downNode.size() > 0) {
                if (!downNode.IsSequence()) {
                    throw new MergeException("Invalid DOWN YAML::Node type");
                }
                setMigration(serial, s, "down", downNode);
            }
        }
    }
    closedir(dir);
}


void Processor::write(const std::string &filename, const nlohmann::json &value) {
    std::ofstream writeStream(filename);

    if (!writeStream.is_open()) {
        throw new MergeException("Unable to open file for writing");
    }

    writeStream << std::setw(4) << value << std::endl;
    writeStream.close();
}

nlohmann::json Processor::read(const std::string &filename) {
    std::ifstream readStream(filename);

    if (!readStream.is_open()) {
        throw new MergeException("Unable to open file for reading");
    }

    nlohmann::json j;
    readStream >> j;

    readStream.close();
    return j;
}

void Processor::setMigration(const int serial, const std::string &filename, const std::string &direction,
                             const YAML::Node &yamlNode) {

    std::cout << "Processor::setMigration()" << std::endl;
    nlohmann::json node = YAMLtoJSON(yamlNode);
    nlohmann::json *migration;
    bool out_of_range = false;

    try {
        migration = &migrationData.at("migrations").at(serial);
    }
    catch (nlohmann::json::out_of_range &e) {
        out_of_range = true;
    }

    if (out_of_range || migration->is_null()) {
        migrationData["migrations"][serial]["serial"] = serial;
        migrationData["migrations"][serial][direction] = nlohmann::json::array();
        migration = &migrationData.at("migrations").at(serial);
    }

    for (json::iterator it = node.begin(); it != node.end(); ++it) {
        (*migration)[direction].push_back(*it);
    }
}

bool Processor::start() {
    std::cout << "Processor::start()" << std::endl;

    if (it.is_null()) {
//    if (it != json::iterator()) {
        it = migrationData.at("migrations").begin();
    } else {
        ++it;
    }

    while (it != migrationData.at("migrations").end() && (*it).is_null()) {
        ++it;
    }

    return (it != migrationData.at("migrations").end());
}

bool Processor::next() {
    std::cout << "Processor::next()" << std::endl;

    if (it != migrationData.at("migrations").end()) {
        ++it;
    }

    while (it != migrationData.at("migrations").end() && (*it).is_null()) {
        ++it;
    }

    return (it != migrationData.at("migrations").end());
}

void Processor::migrate(const std::string &argument) {

    if (!argument.empty()) {
        if (argument.substr(0, 1) == "+") {

            int count = std::stoi(argument.substr(1, std::string::npos));
            int i = 0;

            if (start())
                do {
                    ++i;
                    process((*it)["up"],(*it)["serial"]);
                } while (i < count && next());
        } else {
            int serial = std::stoi(argument);
            int currentSerial = getCurrentSerial();

            if (serial <= currentSerial) {
                std::cerr << "Destination less than current migration." << std::endl;
                return;
            }

            if (start())
                do {
                    int s = (*it)["serial"];
                    if ((*it)["serial"] > serial) break;

                    process((*it)["up"],(*it)["serial"]);
                } while (next());
        }
    } else {
        if (start())
            do {
                process((*it)["up"],(*it)["serial"]);
            } while (next());
    }
}


void Processor::rollback(const std::string &argument) {

    // fs2 rollback
    // fs2 rollback -1
    // fs2 rollback 5
}

void
Processor::merge(nlohmann::json &target, nlohmann::json &patch, const std::string &key, const std::string &path) {

    int i = 0;
    std::map<std::string, int> columnMap;
    std::vector<int> columnDrop;
    std::string matchKey;
    std::string matchValue;
    std::string newPath = path + "/" + key;

    switch (patch.type()) {
        case json::value_t::object:
            std::cout << "newPath: " << path << "key: " << key << " merge object:" << patch.type_name() << std::endl;

            for (json::iterator it = patch.begin(); it != patch.end(); ++it) {
                if (target.count(it.key())) {
                    merge(target[it.key()], *it, it.key(), newPath);
                } else {
                    target[it.key()] = *it;
                }
            }
            break;

        case json::value_t::array:

            matchKey = config.at("sequence").at("merge").at(key);

            for (json::const_iterator it = target.begin(); it != target.end(); ++it) {
                columnMap[it.value().at(matchKey)] = i;
                ++i;
            }

            for (json::iterator it = patch.begin(); it != patch.end(); ++it) {
                matchValue = it.value().at(matchKey);

                if (columnMap.find(matchValue) != columnMap.end()) {

                    try {
                        if ((*it).at("type") == "drop") {
                            target.erase(columnMap[matchValue]);
                            continue;
                        }
                    }
                    catch (nlohmann::json::out_of_range &e) { ;  // do nothing
                    }

                    try {
                        std::string newName = (*it).at("rename");
                        if (columnMap.find(newName) != columnMap.end()) {
                            std::cerr << "Duplicate column name in rename." << std::endl;
                            continue;
                        }
                        target[columnMap[matchValue]][matchKey] = newName;
                        continue;
                    }
                    catch (nlohmann::json::out_of_range &e) { ;  // do nothing
                    }

                    merge(target[columnMap[matchValue]], *it);
                    (*it)["column_add_flag"] = false;
                } else {
                    target.push_back(*it);
                    (*it)["column_add_flag"] = true;
                }
            }

            break;

        case json::value_t::string:
        case json::value_t::boolean:
        case json::value_t::number_float:
        case json::value_t::number_integer:
        case json::value_t::number_unsigned:
            target = patch;
            break;

        default:
            std::cout << "unknown:" << patch.type_name() << std::endl;
            break;
    }
}


void Processor::process(nlohmann::json &migrations, int serial) {
    std::cout << "Processor::process()" << std::endl;

    Action action;

    if (migrations.type() != json::value_t::array)
        throw new MergeException("Not json::value_t::object");

    int i = 0;
    for (json::iterator migration_it = migrations.begin(); migration_it != migrations.end(); ++migration_it, ++i) {

        for (nlohmann::json::const_iterator action_it = actions.begin(); action_it != actions.end(); ++action_it) {
            nlohmann::json target;
            nlohmann::json inja = (*action_it);
            std::string key = action_it.key();

            try {
                target = (*migration_it).at(key);
            }
            catch (nlohmann::json::out_of_range &e) {
                continue;
            }

            std::string targetStr = target;


            char targetFile[PATH_MAX];
            snprintf(targetFile, PATH_MAX, "%s/%s.json", metaDir.c_str(), targetStr.c_str());

            if (file_exists(targetFile)) {
                target = read(targetFile);
            } else {
                char templateFile[PATH_MAX];
                snprintf(templateFile, PATH_MAX, "%s/%s.json", metaDir.c_str(), key.c_str());
                if (file_exists(templateFile)) {
                    target = read(templateFile);
                } else {
                    target = json({});
                }
            }

            merge(target, (*migration_it));

            write(targetFile, target);

            action.doAction(target, *migration_it, *action_it);
            setCurrentSerial(serial);
        }
    }
}
