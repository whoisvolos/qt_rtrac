#pragma once

#include "ObjModel.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

class ObjLoader {

public:
    static ObjModel* loadModel(const char *fileName);

private:

    // model's face
    struct Face
    {
        unsigned int vIndices[3]; // starting with 0

        unsigned int index;

        Face(unsigned int p1_, unsigned int p2_, unsigned int p3_, unsigned int index_) {
            vIndices[0] = p1_;
            vIndices[1] = p2_;
            vIndices[2] = p3_;
            index = index_;
        }

        Face() {}
    };
};
