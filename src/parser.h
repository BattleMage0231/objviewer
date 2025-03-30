#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

struct ObjData {
    std::vector<float> vertices;
    std::vector<unsigned int> faces;
};

class ObjParser {
    std::ifstream s;
    std::stringstream ss;
    ObjData result;

    void parseVertex();
    void parseFace();
public:
    ObjParser(const std::string &path);

    void parse();
    ObjData getResult();
};
