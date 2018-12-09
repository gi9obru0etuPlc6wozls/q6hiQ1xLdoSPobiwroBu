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

using namespace nlohmann;

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

    void scanMigrations();

    void process(const nlohmann::json &migrations);

    void migrate(const std::string &argument);
    void rollback(const std::string &argument);

private:
    typedef bool (Processor::*memberFunction)(const std::string &key, const nlohmann::json &value);

    std::map<std::string, memberFunction> processorFunctions;

    std::string migrationsDir;
    std::string migrationFile;

    nlohmann::json config;
    nlohmann::json migrationData;
    nlohmann::json::iterator it;

    nlohmann::json YAMLtoJSON(const YAML::Node &node);

    void write(const std::string &filename, const nlohmann::json &value);
    nlohmann::json read(const std::string &filename);

    void merge(nlohmann::json &target, const nlohmann::json &patch, const std::string &key = "", const std::string &path = "");

    bool createTable(const std::string &key, const nlohmann::json &value);
    bool alterTable(const std::string &key, const nlohmann::json &value);

    void scanMigrations(std::string &md);
    void setMigration(const int serial, const std::string &filename, const std::string &direction,
                      const YAML::Node &yamlNode);

    nlohmann::json::iterator findMigration(int serial);
    bool start();
    bool next();
    int getCurrentSerial();

};

#endif //FS2_TEMPLATE02_PROCESSOR_H
