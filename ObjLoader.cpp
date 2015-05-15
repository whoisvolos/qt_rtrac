#include "ObjLoader.h"

ObjModel* ObjLoader::loadModel(const char *fileName) {
    using namespace std;

    // Opening file
    ifstream file(fileName);
    if (!file.is_open()) {
        return NULL;
    }

    // Bounding box
    GLfloat minBox[3], maxBox[3];
    minBox[0] = minBox[1] = minBox[2] = 1e10;
    maxBox[0] = maxBox[1] = maxBox[2] = -1e10;

    // DVORNIK-STYLE PARSER
    QVector3D vertex;
    QVector<QVector3D> vertices;
    QVector<Face> faces;
    string str;

    while(getline(file, str)) {
        // Empty line
        if (str == "") {
            continue;
        }

        // Comment
        if (str[0] == '#') {
            continue;
        }

        // Group
        if (str[0] == 'g') {
            continue;
        }

        // Vertex
        if (str[0] == 'v') {
            sscanf(str.c_str() + 1, "%f %f %f", &vertex[0], &vertex[1], &vertex[2]);
            vertices.push_back(vertex);

            // Bounding box
            for (int i = 0; i < 3; i++) {
                if (vertex[i] < minBox[i]) {
                    minBox[i] = vertex[i];
                }
                if (vertex[i] > maxBox[i]) {
                    maxBox[i] = vertex[i];
                }
            }
            continue;
        }

        // Face
        if (str[0] == 'f') {
            QVector<int> objFace;
            string temp;
            stringstream ss(str, ios_base::in);
            ss >> temp;

            // Parsing face line
            while (!ss.eof()) {
                ss >> temp;
                if (temp == "") {
                    break;
                }

                int slashCounter = 0;
                for (int i = 0, l = temp.size(); i < l; ++i) {
                    if (temp[i] == '/') {
                        slashCounter++;
                        temp[i] = ' ';
                    }
                }

                switch (slashCounter) {
                    // Just vertex index
                    default: {
                        int ind;
                        sscanf(temp.c_str(), "%d", &ind);
                        objFace.push_back(ind);
                        break;
                    }
                        // Vertex + normal
                    case 2: {
                        int normalInd, ind, t1, t2, t3;
                        int count = sscanf(temp.c_str(), "%d %d %d", &t1, &t2, &t3);
                        if (count < 3) {
                            sscanf(temp.c_str(), "%d %d", &ind, &normalInd);
                        }
                        else {
                            sscanf(temp.c_str(), "%d %*d %d", &ind, &normalInd);
                        }
                        objFace.push_back(ind);
                        break;
                    }
                }
            }

            // Triangulating
            for (int i = 1; i < objFace.size() - 1; i++) {
                faces.push_back(Face(objFace[0], objFace[i], objFace[i + 1], faces.size()));

                // Shifting face's indices
                faces[faces.size() - 1].vIndices[0] -= 1;
                faces[faces.size() - 1].vIndices[1] -= 1;
                faces[faces.size() - 1].vIndices[2] -= 1;
            }

            continue;
        }
    }

    file.close();

    // Shifting and scaling to bounding box
    GLfloat cubeLen = max(max(maxBox[0] - minBox[0], maxBox[1] - minBox[1]), maxBox[2] - minBox[2]);
    QVector3D transVec = QVector3D((maxBox[0] + minBox[0]) / 2, (maxBox[1] + minBox[1]) / 2, (maxBox[2] + minBox[2]) / 2);
    for (int i = 0, l = vertices.size(); i < l; ++i) {
        vertices[i] -= transVec;
        vertices[i] /= cubeLen;
    }

    // Create model (OpenGL) vectors
    QVector<QVector3D> f;
    for (int i = 0, l = faces.size(); i < l; ++i) {
        for (int j = 0; j < 3; ++j) {
            f.push_back(vertices[faces[i].vIndices[j]]);
        }
    }

    // Normals
    QVector<QVector3D> n;
    for (int i = 0, l = f.size(); i < l; i += 3) {
        auto zerovec = f[i];
        auto norm = QVector3D::crossProduct(f[i + 1] - zerovec, f[i + 2] - zerovec);
        n.push_back(norm);
        n.push_back(norm);
        n.push_back(norm);
    }

    // Final vector
    QVector<QVector3D> *all_data = new QVector<QVector3D>;
    for (int i = 0, l = f.size(); i < l; ++i) {
        all_data->push_back(f[i]);
        all_data->push_back(n[i]);
    }

    auto model = new ObjModel(all_data);
    return model;
}
