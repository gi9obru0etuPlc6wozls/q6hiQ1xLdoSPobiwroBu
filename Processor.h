//
// Created by jwc on 10/19/18.
//

#ifndef FS2_TEMPLATE02_PROCESSOR_H
#define FS2_TEMPLATE02_PROCESSOR_H

#include <iostream>
#include <exception>
#include <yaml-cpp/yaml.h>
#include <yaml-cpp/exceptions.h>
#include <nlohmann/json.hpp>


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

    void process(nlohmann::json &migrations, int serial);

    void migrate(const std::string &argument);
    void rollback(const std::string &argument);

private:

    std::string metaDir;
    std::string migrationsDir;
    std::string migrationFile;

    nlohmann::json config;
    nlohmann::json migrationData;
    nlohmann::json actions;
    nlohmann::json map;
    nlohmann::json::iterator it;

    void write(const std::string &filename, const nlohmann::json &value);
    nlohmann::json read(const std::string &filename);

    void merge(nlohmann::json &target, nlohmann::json &patch, const std::string &key = "", const std::string &path = "");

    void scanMigrations(std::string &md);
    void updateMigrationData(const int serial, const std::string &filename, const std::string &direction,
                             const YAML::Node &yamlNode);

    nlohmann::json::iterator findMigration(int serial);
    bool start();
    bool next();
    int getCurrentSerial();
    void setCurrentSerial(int);

};

#endif //FS2_TEMPLATE02_PROCESSOR_H
