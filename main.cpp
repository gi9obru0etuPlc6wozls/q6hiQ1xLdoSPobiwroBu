#include <iostream>
#include "Processor.h"

int main() {

    Processor p("../config.yaml");
    p.scanMigrations();
    //p.migrate();

    return 0;
}
