#include <iostream>
#include "Processor.h"

class fpclass {

public:

    typedef void (fpclass::*fp_t)();
    std::map<std::string, fp_t> x;

    fp_t a;

    fpclass() {
        std::cout << "hello from fpclass\n";

        x["migration"] = &fpclass::method_a;
        x["create table"] = &fpclass::method_a;

        a = &fpclass::method_a;
    }

    void method_a() {
        std::cout << "hello from method_a\n";
    }

    void method_b() {
        std::cout << "hello from method_b\n";

        (this->*a)();
    }
};




int main() {

    fpclass c;

    c.method_b();

//    Processor p;
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
