//
// Created by jwc on 12/9/18.
//

#ifndef FS2_MIGRATION01_GENERATOR_H
#define FS2_MIGRATION01_GENERATOR_H

#include <yaml-cpp/yaml.h>
#include <assert.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include "YAMLtoJSON.h"
#include <string>
#include <iostream>
#include <yaml-cpp/yaml.h>
#include <yaml-cpp/exceptions.h>
#include <yaml-cpp/node/impl.h>
#include <nlohmann/json.hpp>
#include <inja.hpp>
#include <regex>

using json = nlohmann::json;
using namespace inja;

class Generator {
private:

    Environment env;

    std::string snakeToCamel(const std::string &snake, const bool initCap = false);
public:
    Generator();

    void generate();

};

#endif //FS2_MIGRATION01_GENERATOR_H
