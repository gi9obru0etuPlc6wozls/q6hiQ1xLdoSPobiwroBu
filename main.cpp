#include <iostream>
#include <getopt.h>
#include "Processor.h"

static struct option options[] = {
        {"migrate",  optional_argument, 0, 'm'},
        {"rollback", optional_argument, 0, 'r'},
        {"config",   required_argument, 0, 'c'},
        {"scan",     no_argument,       0, 's'},
        {"help",     no_argument,       0, 'h'},
        {0,          no_argument,       0, 0  },
};

enum OperationEnum {
    none,
    migrate,
    rollback
};

int main(int argc, char **argv) {

    OperationEnum operation = none;
    int scan = false;
    char default_config[] = "../config/config.yaml";
    char *config = default_config;
    char *argument = nullptr;

    while (true) {
        int option_index = 0;
        int c = getopt_long(argc, argv, "", options, &option_index);
        if (c == -1) break;
        printf("c: %c\n",c);
        switch (c) {
            case 's':
                scan = true;
                break;
            case 'c':
                config = optarg;
                break;
            case 'm':
                if (operation != none) {
                    std::cout << "Cannot migrate and rollback at the same time." << std::endl;
                    exit(1);
                }
                argument = optarg;
                operation = migrate;
                break;
            case 'r':
                if (operation != none) {
                    std::cout << "Cannot migrate and rollback at the same time." << std::endl;
                    exit(1);
                }
                argument = optarg;
                operation = rollback;
                break;
            default:
                std::cout << "Invalid command line argument." << std::endl;
                exit(1);
                break;
        }
    }
    std::cout << "Config: " << config << std::endl;
    Processor p(config);

    if (scan) {
        p.scanMigrations();
    }

    switch(operation) {
        case migrate:
            std::cout << "Migrating: " << argument << std::endl;
            p.migrate(argument);
            break;
        case rollback:
            std::cout << "Rolling back: " << argument << std::endl;
            p.rollback(argument);
            break;
        default:
            break;
    }

    return 0;
}
