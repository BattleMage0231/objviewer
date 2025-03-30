#include "parser.h"
#include <iostream>
#include <cctype>
#include <iostream>

ObjParser::ObjParser(const std::string &path) : s {path} {}

void ObjParser::parseVertex() {
    float c;
    for(size_t i = 0; i < 3; ++i) {
        ss >> c;
        result.vertices.emplace_back(c);
    }
}

void ObjParser::parseFace() {
    int idx;
    while(ss >> idx) {
        if(idx >= 0) {
            result.faces.emplace_back(static_cast<unsigned int>(idx));
        } else {
            result.faces.emplace_back(static_cast<unsigned int>(result.faces.size() + idx));
        }
    }

    result.faces.emplace_back(0xFFFF);
}

void ObjParser::parse() {
    std::string line, token;
    while(std::getline(s, line)) {
        ss = std::stringstream {line};
        if(!(ss >> token)) continue;
        if(token == "v") {
            parseVertex();
        } else if(token == "f") {
            parseFace();
        }
    }
}

ObjData ObjParser::getResult() {
    return result;
}
