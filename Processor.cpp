//
// Created by jwc on 10/19/18.
#include <fstream>
#include <iomanip>
#include <dirent.h>
#include <sys/stat.h>
#include "Processor.h"

nlohmann::json Processor::YAMLtoJSON(const YAML::Node &node) {
    int i = 0;
    nlohmann::json data;

    switch (node.Type()) {
        case YAML::NodeType::Null: // ...
            data = nullptr;
            break;
        case YAML::NodeType::Scalar: // ...
            try {
                data = node.as<bool>();
            }
            catch (YAML::BadConversion &x) {
                try {
                    data = node.as<int>();
                }
                catch (YAML::BadConversion &x) {
                    try {
                        data = node.as<double>();
                    }
                    catch (YAML::BadConversion &x) {
                        data = node.as<std::string>();
                    }
                }
            }
            break;
        case YAML::NodeType::Sequence: // ...
            if (node.begin() == node.end()) {
                data = json::array();
            } else {
                for (YAML::const_iterator n_it = node.begin(); n_it != node.end(); ++n_it) {
                    data[i] = YAMLtoJSON(*n_it);
                    ++i;
                }
            }
            break;
        case YAML::NodeType::Map: // ...
            for (YAML::const_iterator n_it = node.begin(); n_it != node.end(); ++n_it) {
                data[n_it->first.as<std::string>()] = YAMLtoJSON(n_it->second);
            }
            break;
        case YAML::NodeType::Undefined: // ...
            break;
    }

    return data;
}

Processor::Processor(std::string filename) {
    std::cout << "Processor::Processor()" << std::endl;

    processorFunctions["migration"] = &Processor::migration;
    processorFunctions["create table"] = &Processor::createTable;
    processorFunctions["alter table"] = &Processor::alterTable;

    config = YAMLtoJSON(YAML::LoadFile(filename));
    std::cout << "config: " << config.dump(4) << std::endl;

    migrationsDir = config.at("paths").at("migrations dir");

    std::string batchFile = config.at("paths").at("batch file");
    batch = read(batchFile);
    std::cout << "batch: " << batch.dump(4) << std::endl;
}

void Processor::merge(nlohmann::json &target, const nlohmann::json &patch, const std::string &key) {

    int i = 0;
    std::map<std::string, int> columnMap;
    std::string matchKey;
    std::string matchValue;

    switch (patch.type()) {
        case json::value_t::object:
            std::cout << "key: " << key << " merge object:" << patch.type_name() << std::endl;
            for (json::const_iterator it = patch.begin(); it != patch.end(); ++it) {
                if (target.count(it.key())) {
                    merge(target[it.key()], *it, it.key());
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

            for (json::const_iterator it = patch.begin(); it != patch.end(); ++it) {
                matchValue = it.value().at(matchKey);

                if (columnMap.find(matchValue) != columnMap.end()) {
                    merge(target[columnMap[matchValue]], *it);
                } else {
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

/*
 ************************************************
 *
 * User input:
 *  direction: up/down
 *  batch number: [+/-]###
 *
 *  batch.json
 *   current batch: ###
 *   array of batches: batch #, filenames
 *
 *  migration files
 */


void Processor::scanMigrations(std::string direction) {
    scanMigrations(migrationsDir, direction);
}

void Processor::scanMigrations(std::string &md, std::string &direction) {

    std::cout << "Processor::scanMigrations(): " << md << std::endl;

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
            scanMigrations(s, direction);
        } else if (!S_ISREG(fileInfo.st_mode)) {
            throw new MergeException("Unknown file type");
        }

        if (strlen(dirent->d_name) > 5 && !strcmp(dirent->d_name + strlen(dirent->d_name) - 5, ".yaml")) {
            YAML::Node migration = YAML::LoadFile(s)["migration"];

            if (!migration.IsMap()) {
                std::cout << "Migration key not found, skipping: " << s << std::endl;
                continue;
            }

            YAML::Node batchNode = migration["batch"];

            if (!batchNode.IsScalar()) {
                std::cout << "Error getting batch, skipping: " << s << std::endl;
                continue;
            }

            int fileBatch = batchNode.as<int>();

            YAML::Node directionNode = migration["direction"];

            if (!batchNode.IsScalar()) {
                std::cout << "Error getting direction, skipping: " << s << std::endl;
                continue;
            }

            std::string fileDirection = directionNode.as<std::string>();

            if (fileDirection != direction) continue;

            std::cout << "batch: " << batch.dump(4) << std::endl;
            std::cout << "s: " << s << std::endl;

            batch["batches"][fileBatch]["date"] = "20181031 12:34:56";
            batch["batches"][fileBatch]["files"] = json::array();
            batch["batches"][fileBatch]["files"].push_back(s);

            std::cout << "batch: " << batch.dump(4) << std::endl;


        }
    }
    closedir(dir);
}

/*
 * process a single migration file
 */
void Processor::process(const nlohmann::json &source) {

    std::cout << "Processor::process()" << std::endl;

    if (source.type() != json::value_t::object)
        throw new MergeException("Not json::value_t::object");

    for (json::const_iterator n_it = source.begin(); n_it != source.end(); ++n_it) {
        std::string key = n_it.key();

        std::cout << "key: " << key << std::endl;

        if (processorFunctions.find(key) == processorFunctions.end()) {
            throw new MergeException("No matching processorFunction");
        }

        if (!(this->*processorFunctions[key])(key, n_it.value())) {
            throw new MergeException("processorFunction returned error");
        }
    }
}

bool Processor::migration(const std::string &key, const nlohmann::json &value) {
    std::cout << "Processor::migration()" << std::endl;

    //scanmigrations(migrationsDir);

    nlohmann::json target;

    merge(target, value);

    return true;
}

bool Processor::createTable(const std::string &key, const nlohmann::json &value) {
    std::cout << "Processor::createlTable()" << std::endl;
    std::cout << "value: " << value.dump(4) << std::endl;
    nlohmann::json target;

    try {
        target = config.at("template").at(key);
    }
    catch (nlohmann::json::out_of_range &e) {
        // Do nothing
    }

    std::cout << "target: " << target.dump(4) << std::endl;
    merge(target, value);
    std::cout << "target: " << target.dump(4) << std::endl;

    write("target.json", target);

    return true;
}

bool Processor::alterTable(const std::string &key, const nlohmann::json &value) {
    std::cout << "Processor::alterTable()" << std::endl;

    nlohmann::json target = read("target.json");
    std::cout << "target: " << target.dump(4) << std::endl;
    merge(target, value);
    std::cout << "target: " << target.dump(4) << std::endl;

    write("target.json", target);

    return true;
}