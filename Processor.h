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

    Processor(std::string filename);

    void process(const nlohmann::json &source);

    void scanMigrations();

private:
    typedef bool (Processor::*memberFunction)(const std::string &key, const nlohmann::json &value);

    std::map<std::string, memberFunction> processorFunctions;

    std::string migrationsDir;
    std::string batchFile;

    nlohmann::json config;
    nlohmann::json batches;

    nlohmann::json YAMLtoJSON(const YAML::Node &node);

    void write(const std::string &filename, const nlohmann::json &value);
    nlohmann::json read(const std::string &filename);

    void merge(nlohmann::json &target, const nlohmann::json &patch, const std::string &key = "", const std::string &path = "");

    bool migration(const std::string &key, const nlohmann::json &value);
    bool createTable(const std::string &key, const nlohmann::json &value);
    bool alterTable(const std::string &key, const nlohmann::json &value);

    void scanMigrations(std::string &md);
    void updateBatch(const int serial, const std::string &filename, const std::string &direction, const YAML::Node &yamlNode);
};

#endif //FS2_TEMPLATE02_PROCESSOR_H
