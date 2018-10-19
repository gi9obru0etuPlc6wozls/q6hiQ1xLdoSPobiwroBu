#include <iostream>
#include "Processor.h"

void f() {
    std::cout << "I'm in function f" << std::endl;
}

typedef void (*fpt)();

class fpclass {

    typedef  void (fpclass::*mfp_t)();

public:
    void (*fpa)();// = &fpclass::method_a; // &myfoo::foo;

    void method_a() {
        std::cout << "hello from method_a\n";
    }

    void method_b() {

        mfp_t p = &fpclass::method_a;   // FredMemFn p = &Fred::f;

                p();
        std::cout << "hello from method_b\n";
    }
};



int main() {

    fpclass fpc;

    fpc.method_b();

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
