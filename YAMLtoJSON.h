//
// Created by jwc on 12/9/18.
//

#ifndef FS2_MIGRATION01_YAMLTOJSON_H
#define FS2_MIGRATION01_YAMLTOJSON_H


#include <nlohmann/json.hpp>

nlohmann::json YAMLtoJSON(const YAML::Node &node);

#endif //FS2_MIGRATION01_YAMLTOJSON_H
