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

    void process(const char *filename);
//    void process(const std::string &filename);
    void process(const nlohmann::json &source);

private:
    typedef bool (Processor::*memberFunction)(const std::string &key, const nlohmann::json &value);
    std::map<std::string, memberFunction> processorFunctions;

    nlohmann::json config;

    nlohmann::json YAMLtoJSON(const YAML::Node &node);

    void merge(nlohmann::json &target, const nlohmann::json &patch, const std::string &key = "");
    void mergeSequence(nlohmann::json &target, const nlohmann::json &patch, const std::string &key = "name");

    bool migration(const std::string &key, const nlohmann::json &value);
    bool createTable(const std::string &key, const nlohmann::json &value);
    bool alterTable(const std::string &key, const nlohmann::json &value);
};

#endif //FS2_TEMPLATE02_PROCESSOR_H
