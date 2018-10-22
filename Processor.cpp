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

Processor::Processor() {
    std::cout << "Processor::Processor()" << std::endl;
    processorFunctions["migration"] = &Processor::migration;
    processorFunctions["create table"] = &Processor::createTable;
    processorFunctions["alter table"] = &Processor::alterTable;
}

Processor::Processor(std::string filename) : Processor() {
    loadConfig(filename);
}

Processor::Processor(nlohmann::json &config) : Processor() {
    this->config = config;
}

void Processor::loadConfig(std::string filename) {
    config = YAMLtoJSON(YAML::LoadFile(filename));
    std::cout << "config: " << config.dump(4) << std::endl;
}

void Processor::process(const char *const filename) {
    std::string fn = filename;
    process(YAMLtoJSON(YAML::LoadFile(fn)));
}

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

void Processor::scanmigrations(const std::string &dirname) {
    std::cout << "Processor::scanmigrations(): " << dirname << std::endl;

    DIR *dir;
    struct dirent *dirent;
    struct stat fileInfo;

    if ((dir = opendir(dirname.c_str())) == NULL) {
        throw new MergeException("Could not open directory.");
    }

    while ((dirent = readdir(dir)) != NULL) {
        std::string s = dirname + "/" + dirent->d_name;
        std::cout << "s: " << s << std::endl;

        if (strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..") == 0) {
            continue;
        }

        stat(s.c_str(), &fileInfo);
        if (S_ISDIR(fileInfo.st_mode)) {
            scanmigrations(s);
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

bool Processor::migration(const std::string &key, const nlohmann::json &value) {
    std::cout << "Processor::migration()" << std::endl;

    scanmigrations(".");

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