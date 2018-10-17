#include <iostream>
#include <yaml-cpp/yaml.h>
#include <yaml-cpp/exceptions.h>
#include <yaml-cpp/node/impl.h>
#include <nlohmann/json.hpp>
#include <inja.hpp>
#include <regex>

using json = nlohmann::json;
using namespace inja;

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <iomanip> // for std::setw

using json = nlohmann::json;

nlohmann::json YAMLtoJSON(const YAML::Node &node) {
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

class MergeException : std::exception {
private:
    char const *e;

public:
    MergeException(char const *e) {
        this->e =  e;
    }

    virtual const char *what() const throw() {
        return e;
    }
};

void merge(nlohmann::json &target, const nlohmann::json &patch);
void merge_columns(nlohmann::json &target, const nlohmann::json &patch);

void merge_columns(nlohmann::json &target, const nlohmann::json &patch) {

    std::map<std::string, int> columnMap;

    std::string column[3] = {"name", "precision", "not null"}; // TODO: Make a config

    int i = 0;

    if (target.type() != json::value_t::array || patch.type() != json::value_t::array)
        throw new MergeException("Not json::value_t::array");

    for (json::const_iterator it = target.begin(); it != target.end(); ++it) {
        columnMap[it.value().at("name")] = i;
        //std::cout << "Name: " << it.value().type_name() << std::endl;
        ++i;
    }

    for (json::const_iterator it = patch.begin(); it != patch.end(); ++it) {
        std::string key = it.value().at("name");

        if (columnMap.find(key) != columnMap.end()) {
            merge(target[columnMap[key]], *it);
        } else {
            target.push_back(*it);
        }
    }
}

void merge(nlohmann::json &target, const nlohmann::json &patch) {

    int i = 0;

    switch (patch.type()) {
        case json::value_t::object:
            for (json::const_iterator it = patch.begin(); it != patch.end(); ++it) {
                if (it.key() == "columns") {
                    merge_columns(target[it.key()], *it);
                } else if (target.count(it.key())) {
                    merge(target[it.key()], *it);
                } else {
                    target[it.key()] = *it;
                }
            }
            break;

        case json::value_t::array:
            std::cout << "not supported:" << patch.type_name() << std::endl;
//            throw new std::exception(); // TODO:: how to merge generic arrays?
//            std::cout << std::string(level * 4, ' ') << "array:" << patch.type_name() << std::endl;
//            for (json::const_iterator it = patch.begin(); it != patch.end(); ++it) {
//                std::cout << std::string(level * 4, ' ') << "Element:" << ++i;
//            }
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


int main() {

    YAML::Node targetYaml;

    try {
        targetYaml = YAML::LoadFile("../target.json");
    }
    catch (YAML::BadFile &e) {
        // TODO: Initalize with defaults from YAML config?
        targetYaml["columns"] = YAML::Load("[]");
        std::cout << "targetYaml: " << targetYaml << std::endl;
        std::cout << "BadFile exception" << std::endl;
    }

    YAML::Node patchYaml = YAML::LoadFile("../test02.yaml");

    nlohmann::json targetJson = YAMLtoJSON(targetYaml);
    nlohmann::json patchJson = YAMLtoJSON(patchYaml);
    merge(targetJson, patchJson);

    std::cout << std::endl;
    std::cout << "Target: " << targetJson.dump(4) << std::endl;

    std::ofstream targetFile("../target.json");
    targetFile << targetJson.dump(4);
    targetFile.close();

    return 0;
}
