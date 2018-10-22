#include <iostream>
#include "Processor.h"

int main() {

    Processor p("../config.yaml");
    p.process("../test02.yaml");

    return 0;
}
