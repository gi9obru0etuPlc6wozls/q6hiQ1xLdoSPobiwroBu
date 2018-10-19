//
// Created by jwc on 10/19/18.
//

#ifndef FS2_TEMPLATE02_PROCESSOR_H
#define FS2_TEMPLATE02_PROCESSOR_H

#include <iostream>
#include <exception>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <yaml-cpp/exceptions.h>

using json = nlohmann::json;

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


class Processor {
public:


    Processor();
    Processor(std::string filename);
    Processor(nlohmann::json &config);

    void loadConfig(std::string filename);

    void process(std::string filename);
    void process(const nlohmann::json &source);

private:

    nlohmann::json config;

    nlohmann::json YAMLtoJSON(const YAML::Node &node);

    void merge(nlohmann::json &target, const nlohmann::json &patch, const std::string &key = "");
    void merge_sequence(nlohmann::json &target, const nlohmann::json &patch, const std::string &key = "name");
};

#endif //FS2_TEMPLATE02_PROCESSOR_H
