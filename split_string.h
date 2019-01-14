//
// Created by jwc on 1/12/19.
//

#ifndef FS2_MIGRATION02_SPLIT_STRING_H
#define FS2_MIGRATION02_SPLIT_STRING_H


std::vector<std::string> split(const std::string& str, const std::string& delimiter)
{
    std::vector<std::string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delimiter, prev);
        if (pos == std::string::npos) pos = str.length();
        std::string token = str.substr(prev, pos-prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delimiter.length();
    }
    while (pos < str.length() && prev < str.length());
    return tokens;
}


#endif //FS2_MIGRATION02_SPLIT_STRING_H
