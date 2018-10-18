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
        this->e = e;
    }

    virtual const char *what() const throw() {
        return e;
    }
};

void merge(nlohmann::json &target, const nlohmann::json &patch, const std::string &key = "");

void merge(nlohmann::json &target, const nlohmann::json &patch, const std::string &key) {

    int i = 0;
    std::map<std::string, int> columnMap;

    switch (patch.type()) {
        case json::value_t::object:
            for (json::const_iterator it = patch.begin(); it != patch.end(); ++it) {
                if (target.count(it.key())) {
                    merge(target[it.key()], *it, it.key());
                } else {
                    target[it.key()] = *it;
                }
            }
            break;

        case json::value_t::array:
            std::cout << "key: " << key << " not supported:" << patch.type_name() << std::endl;

            if (target.type() != json::value_t::array)
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

    nlohmann::json config = YAMLtoJSON(YAML::LoadFile("../config.yaml"));
    std::cout << "config: " << config.dump(4) << std::endl;

    YAML::Node targetYaml;

    try {
        targetYaml = YAML::LoadFile("../test01.yaml");
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
