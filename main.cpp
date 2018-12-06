#include <iostream>
#include "Processor.h"

int main() {

    Processor p("../config.yaml"); // <---
    p.scanMigrations(); // // fs2 scan

    p.migrate("+1");

    // fs2 migrate
    // fs2 migrate +1
    // fs2 migrate 5


    return 0;
}
