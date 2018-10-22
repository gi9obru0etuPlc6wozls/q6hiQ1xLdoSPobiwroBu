#include <iostream>
#include "Processor.h"

int main() {

    Processor p("../config.yaml");
    p.process("../test02.yaml");

//
//
//    p.loadConfig("../config.yaml");
//
//    std::string patch = "../test01.yaml";
//
//    fpt fp = &f;
//
//    std::map<std::string, fpt> fm;
//
//    fm["dylan"] = &f;
//
//    f();
//    (*fp)();
//    (*fm["dylan"])();
//    fm["dylan"]();



//
//    YAML::Node targetYaml;
//
//    try {
//        targetYaml = YAML::LoadFile("../test01.yaml");
//    }
//    catch (YAML::BadFile &e) {
//        // TODO: Initalize with defaults from YAML config?
//        targetYaml["columns"] = YAML::Load("[]");
//        std::cout << "targetYaml: " << targetYaml << std::endl;
//        std::cout << "BadFile exception" << std::endl;
//    }
//
//    YAML::Node patchYaml = YAML::LoadFile("../test02.yaml");
//
//    nlohmann::json targetJson = YAMLtoJSON(targetYaml);
//    nlohmann::json patchJson = YAMLtoJSON(patchYaml);
//    merge(targetJson, patchJson);
//
//    std::cout << std::endl;
//    std::cout << "Target: " << targetJson.dump(4) << std::endl;
//
//    std::ofstream targetFile("../target.json");
//    targetFile << targetJson.dump(4);
//    targetFile.close();

    return 0;
}
