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
    //std::cout << "config: " << config.dump(4) << std::endl;

    metaDir = config.at("paths").at("meta dir");
    migrationsDir = config.at("paths").at("migrations dir");
    migrationFile = config.at("paths").at("migrations file");

    actions = config.at("actions");

    try {
        migrationData = read(migrationFile);
    }
    catch (MergeException *e) {
        migrationData = config.at("migration data");
        write(migrationFile, migrationData);
    }

    int currentSerial = getCurrentSerial();
    if (currentSerial != -1) {
        std::cout << "setting it to current serial: " << currentSerial << std::endl;
        it = findMigration(currentSerial);
    }

    //std::cout << "Processor::Processor migrationData: " << migrationData.dump(4) << std::endl;
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
        std::cout << "Current migration is null" << std::endl;
        return -1;
    }

    int currentSerial = migrationCurrent;
    it = findMigration(currentSerial);

    return (*it)["serial"];
}

nlohmann::json::iterator Processor::findMigration(int targetSerial) {
    std::cout << "Processor::findMigration()" << std::endl;

    nlohmann::json::iterator migrationIt;
    nlohmann::json *migrations = &migrationData.at("migrations");

//    nlohmann::json migrationCurrent = migrationData.at("current");
//
//    if (migrationCurrent.is_null()) {
//        std::cout << "Current migration is null" << std::endl;
//        return migrationIt;
//    }
//
//    int currentIndex = migrationCurrent;
//
//    if ((*migrations).at(currentIndex).is_null()) {
//        throw new MergeException("Migration that current points to is null.");
//    }

    for (migrationIt = (*migrations).begin(); migrationIt != (*migrations).end(); ++migrationIt) {
        if ((*migrationIt).is_null()) {
            continue;
        }
        int serial = (*migrationIt).at("serial");
        if (targetSerial == serial) {
            std::cout << "found serial: " << serial << std::endl;
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
            std::cout << "serial: " << serial << std::endl;

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
        std::cout << "found migration: " << (*migration).dump(4) << std::endl;
    }
    catch (nlohmann::json::out_of_range &e) {
        out_of_range = true;
    }

    if (out_of_range || migration->is_null()) {
        migrationData["migrations"][serial]["serial"] = serial;
        migrationData["migrations"][serial][direction] = nlohmann::json::array();
        migration = &migrationData.at("migrations").at(serial);
        std::cout << "create migration: " << (*migration).dump(4) << std::endl;
    }

    for (json::iterator it = node.begin(); it != node.end(); ++it) {
        (*migration)[direction].push_back(*it);
    }
}

bool Processor::start() {
    std::cout << "Processor::start()" << std::endl;

    if (it.is_null()) {
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
                    std::cout << "1 Migration:" << (*it) << std::endl;
                    process((*it)["up"]);
                } while (i < count && next());
        } else {
            int serial = std::stoi(argument);
            int currentSerial = getCurrentSerial();
            std::cout << "Current: " << currentSerial << " Destination: " << serial << std::endl;

            if (serial <= currentSerial) {
                std::cout << "Destination less than current migration." << std::endl;
                return;
            }

            if (start())
                do {
                    int s = (*it)["serial"];
                    if ((*it)["serial"] > serial) break;

                    std::cout << "s: " << s << " serial: " << serial << std::endl;
                    std::cout << "2 Migration:" << (*it) << std::endl;
                    process((*it)["up"]);
                } while (next());
        }
    }
    else {
        if (start())
            do {
                std::cout << "3 Migration:" << (*it) << std::endl;
                process((*it)["up"]);
            } while (next());
    }

    // fs2 migrate
    // fs2 migrate +1
    // fs2 migrate 5
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
                    std::cout << "Assigning: " << std::endl;
                    target[it.key()] = *it;
                }
            }
            break;

        case json::value_t::array:
            std::cout << "key: " << key << " merge array:" << patch.type_name() << std::endl;

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
                            std::cout << "array it: " << (*it) << std::endl;
                            target.erase(columnMap[matchValue]);
                            continue;
                        }
                    }
                    catch (nlohmann::json::out_of_range &e) {
                        ;
                    }

                    try {
                        std::string newName = (*it).at("rename");
                        std::cout << "array it: " << (*it) << std::endl;
                        if (columnMap.find(newName) != columnMap.end()) {
                            std::cerr << "Duplicate column name in rename." << std::endl;
                            continue;
                        }
                        target[columnMap[matchValue]][matchKey] = newName;
                        std::cout << "target matchKey: " << target[columnMap[matchValue]][matchKey] << std::endl;
                        continue;
                    }
                    catch (nlohmann::json::out_of_range &e) {
                        ;
                    }

                    merge(target[columnMap[matchValue]], *it);
                } else {
                    nlohmann::json o;
                    o["add"] = true;



                    target.push_back(*it);
                }
            }

            break;

        case json::value_t::string:
        case json::value_t::boolean:
        case json::value_t::number_float:
        case json::value_t::number_integer:
        case json::value_t::number_unsigned:
            std::cout << "key: " << key << " merge scalar:" << patch.type_name() << std::endl;
            target = patch;
            break;

        default:
            std::cout << "unknown:" << patch.type_name() << std::endl;
            break;
    }
}


void Processor::process(nlohmann::json &migrations) {
    std::cout << "Processor::process()" << std::endl;

    std::cout << "migrations: " << migrations.dump(4) << std::endl;

    Action action;
    std::cout << "templates: " << actions.dump(4) << std::endl;

    //std::snprintf()

    if (migrations.type() != json::value_t::array)
        throw new MergeException("Not json::value_t::object");

    int i = 0;
    for (json::iterator migration_it = migrations.begin(); migration_it != migrations.end(); ++migration_it, ++i) {

        for (nlohmann::json::const_iterator action_it = actions.begin(); action_it != actions.end(); ++action_it) {
            nlohmann::json target;
            nlohmann::json inja = (*action_it);
            std::string key = action_it.key();

            std::cout << "Inja key:" << key <<  " i: " << i << std::endl;

            try {
                target = (*migration_it).at(key);
            }
            catch (nlohmann::json::out_of_range &e) {
                continue;
            }

            std::cout << "--- Running Actions: " << std::endl;

            std::string targetStr = target;

            std::cout << "Found key: " << key << std::endl;
            std::cout << "Target: " << target << std::endl;

            char targetFile[PATH_MAX];
            snprintf(targetFile, PATH_MAX, "%s/%s.json", metaDir.c_str(), targetStr.c_str());
            std::cout << "targetFile: " << targetFile << std::endl;

            if (file_exists(targetFile)) {
                target = read(targetFile);
            }
            else {
                char templateFile[PATH_MAX];
                snprintf(templateFile, PATH_MAX, "%s/%s.json", metaDir.c_str(), key.c_str());
                std::cout << "templateFile: " << templateFile << std::endl;
                if (file_exists(templateFile)) {
                    target = read(templateFile);
                } else {
                    target = json({});
                }
            }
            std::cout << "Pre-merge Target: " << target.dump(4) << std::endl;

            merge(target, (*migration_it));

            std::cout << "Post-merge Target: " << target.dump(4) << std::endl;

            write(targetFile, target);

            action.runAction(target, *migration_it, *action_it);

        }
//
//        if (actionFunctions.find(key) == actionFunctions.end()) {
//            throw new MergeException("No matching processorFunction");
//        }
//
//        if (!(this->*actionFunctions[key])(key, migration_it.value())) {
//            throw new MergeException("processorFunction returned error");
//        }
    }
}
