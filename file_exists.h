//
// Created by jwc on 12/13/18.
//

#ifndef FS2_MIGRATION01_FILE_EXISTS_H
#define FS2_MIGRATION01_FILE_EXISTS_H

#include <string>
#include <sys/stat.h>

inline bool file_exists (const std::string& name) {
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
}

#endif //FS2_MIGRATION01_FILE_EXISTS_H
