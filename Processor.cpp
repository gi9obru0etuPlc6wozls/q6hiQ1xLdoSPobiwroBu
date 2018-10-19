//
// Created by jwc on 10/19/18.
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

void Processor::loadConfig(std::string filename) {
    config = YAMLtoJSON(YAML::LoadFile(filename));
    std::cout << "config: " << config.dump(4) << std::endl;
}

Processor::Processor() {}

Processor::Processor(std::string filename) {
    loadConfig(filename);
}

Processor::Processor(nlohmann::json &config) {
    this->config = config;
}

void Processor::process(std::string filename) {
    process(YAMLtoJSON(YAML::LoadFile(filename)));
}

void Processor::process(const nlohmann::json &source) {

    if (source.type() != json::value_t::object)
        throw new MergeException("Not json::value_t::object");

    for (json::const_iterator it = source.begin(); it != source.end(); ++it) {

    }
}

void Processor::merge_sequence(nlohmann::json &target, const nlohmann::json &patch, const std::string &mergeKey) {
    int i = 0;
    std::map<std::string, int> columnMap;
    std::string key;

    for (json::const_iterator it = target.begin(); it != target.end(); ++it) {
        columnMap[it.value().at("name")] = i;
        ++i;
    }

    for (json::const_iterator it = patch.begin(); it != patch.end(); ++it) {
        key = it.value().at("name");

        if (columnMap.find(key) != columnMap.end()) {
            merge(target[columnMap[key]], *it);
        } else {
            target.push_back(*it);
        }
    }
}


void Processor::merge(nlohmann::json &target, const nlohmann::json &patch, const std::string &key) {

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
            std::cout << "key: " << key << " merge sequence:" << patch.type_name() << std::endl;

            if (target.type() != json::value_t::array)
                throw new MergeException("Not json::value_t::array");

            merge_sequence(target, patch);

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