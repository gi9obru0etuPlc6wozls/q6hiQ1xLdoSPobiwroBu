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
#include <nlohmann/json.hpp>
#include <iomanip> // for std::setw

using json = nlohmann::json;

static int level = 0;

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
            for (YAML::const_iterator n_it = node.begin(); n_it != node.end(); ++n_it) {
                data[i] = YAMLtoJSON(*n_it);
                ++i;
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

nlohmann::json YAMLdebug(const YAML::Node &node) {
    int i = 0;
    nlohmann::json data;

    level++;
    std::cout << std::string(level * 4, ' ') << "";

    switch (node.Type()) {
        case YAML::NodeType::Null: // ...
            std::cout << "Null" << std::endl;
            data = nullptr;
            break;
        case YAML::NodeType::Scalar: // ...
            std::cout << "Scalar: ";
            try {
                std::cout << "bool: " << node.as<bool>();
                data = node.as<bool>();
            }
            catch (YAML::BadConversion &x) {
                try {
                    std::cout << "int: " << node.as<int>();
                    data = node.as<int>();
                }
                catch (YAML::BadConversion &x) {
                    try {
                        data = node.as<double>();
                        std::cout << "double: " << node.as<double>();
                    }
                    catch (YAML::BadConversion &x) {
                        std::cout << "string: " << node.as<std::string>();
                        data = node.as<std::string>();
                    }
                }
            }
            std::cout << std::endl;
            break;
        case YAML::NodeType::Sequence: // ...
            std::cout << "Sequence" << std::endl;
            for (YAML::const_iterator n_it = node.begin(); n_it != node.end(); ++n_it) {
                std::cout << std::string(level * 4, ' ') << "Element:" << i; // << std::endl;
                data[i] = YAMLdebug(*n_it);
                ++i;
            }
            break;
        case YAML::NodeType::Map: // ...
            std::cout << "Map" << std::endl;
            for (YAML::const_iterator n_it = node.begin(); n_it != node.end(); ++n_it) {
                std::cout << std::string(level * 4, ' ') << "Key:" << n_it->first.as<std::string>(); // << std::endl;
                data[n_it->first.as<std::string>()] = YAMLdebug(n_it->second);
            }
            break;
        case YAML::NodeType::Undefined: // ...
            std::cout << "Undefined" << std::endl;
            break;
    }

    level--;

    return data;
}


nlohmann::json merge(nlohmann::json &target, const nlohmann::json &patch);

nlohmann::json merge_columns(nlohmann::json &target, const nlohmann::json &patch) {

    std::map<std::string, int> columnMap;

    int i = 0;

    if (target.type() != json::value_t::array || patch.type() != json::value_t::array)
        throw "Columns must be array";

    for (json::const_iterator it = target.begin(); it != target.end(); ++it) {
        columnMap[it.value().at("name")] = i;
        ++i;
    }
    for (json::const_iterator it = patch.begin(); it != patch.end(); ++it) {
        std::string key = it.value().at("name");

        if (columnMap.find(key) != columnMap.end()) {
            merge(target[columnMap[key]], *it);
        }
        else {
            target.push_back(*it);
        }
    }
}

nlohmann::json merge(nlohmann::json &target, const nlohmann::json &patch) {

    int i = 0;

    level++;

    switch (patch.type()) {
        case json::value_t::object:
            for (json::const_iterator it = patch.begin(); it != patch.end(); ++it) {
                std::string key = it.key();

                std::cout << std::string(level * 4, ' ') << "Key:" << key;  // TODO: fails with out this line

                if (key == "columns") {
                    merge_columns(target[key], *it);
                } else if (target.count(key)) {
                    merge(target[key], *it);
                } else {
                    target[key] = *it;
                }
            }
            break;

        case json::value_t::array:
            throw new std::exception(); // TODO:: how to merge generic arrays?
            std::cout << std::string(level * 4, ' ') << "array:" << patch.type_name() << std::endl;
            for (json::const_iterator it = patch.begin(); it != patch.end(); ++it) {
                std::cout << std::string(level * 4, ' ') << "Element:" << ++i;
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
            std::cout << std::string(level * 4, ' ') << "unknown:" << patch.type_name();
            break;
    }

    std::cout << std::endl;
    level--;


}


int main() {
    YAML::Node targetYaml = YAML::LoadFile("../test01.yaml");
    YAML::Node patchYaml = YAML::LoadFile("../test02.yaml");

    std::cout << std::endl;

    nlohmann::json target = YAMLtoJSON(targetYaml);
    nlohmann::json patch = YAMLtoJSON(patchYaml);
    merge(target, patch);

    std::cout << "Target: " << target.dump(4) << std::endl;
}
