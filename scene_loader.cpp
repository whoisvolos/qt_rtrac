#include "scene_loader.h"

scene_t* load_scene(const char *filePath, int post_process) {
    using namespace std;

    ifstream file(filePath);
    if (!file.is_open()) {
        return 0;
    }

    point_t minBox[3], maxBox[3];
    minBox[0] = minBox[1] = minBox[2] = 1e10;
    maxBox[0] = maxBox[1] = maxBox[2] = -1e10;

    vec3 vertex;
    vector<vec3> vertices;
    vector<idx_face_t> faces;
    string str;

    while(getline(file, str)) {
        if (str == "") {
            continue;
        } else {
            switch (str[0]) {
                case 'v': {
                    sscanf(str.c_str() + 1, "%f %f %f", &(vertex.x), &(vertex.y), &(vertex.z));
                    vertices.push_back(vertex);
                    for (int i = 0; i < 3; ++i) {
                        point_t vi = *(((point_t *)&vertex) + i);
                        if (vi < minBox[i]) {
                            minBox[i] = vi;
                        }
                        if (vi > maxBox[i]) {
                            maxBox[i] = vi;
                        }
                    }
                    break;
                }
                case 'f': {
                    vector<int> objFace;
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
                            default: {
                                int ind;
                                sscanf(temp.c_str(), "%d", &ind);
                                objFace.push_back(ind);
                                break;
                            }
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
                        faces.push_back(idx_face_t(objFace[0], objFace[i], objFace[i + 1], faces.size()));
                        faces[faces.size() - 1].indices[0] -= 1;
                        faces[faces.size() - 1].indices[1] -= 1;
                        faces[faces.size() - 1].indices[2] -= 1;
                    }

                    break;
                }
                default: {
                    break;
                }
            }
        }
    }
    
    file.close();

    if ((post_process & SCENE_POSTPROCESS_SCALE_AND_MOVE) == 1) {
        point_t cubeLen = max(max(maxBox[0] - minBox[0], maxBox[1] - minBox[1]), maxBox[2] - minBox[2]);
        vec3 translation_vec = make_vec((maxBox[0] + minBox[0]) / 2, (maxBox[1] + minBox[1]) / 2, (maxBox[2] + minBox[2]) / 2);
        for (int i = 0, l = vertices.size(); i < l; ++i) {
            vertices[i] -= translation_vec;
            vertices[i] /= cubeLen;
        }
    }

    scene_t *result = (scene_t *)malloc(sizeof(scene_t));
    result->n_meshes = 1;
    result->meshes = (mesh_t *)malloc(sizeof(mesh_t));
    result->meshes[0].first_idx = 0;
    result->n_faces = result->meshes[0].n_faces = faces.size();
    result->faces = (face_t *)calloc(result->n_faces, sizeof(face_t));
    for (int i = 0; i < result->n_faces; ++i) {
        idx_face_t idx_face = faces[i];
        result->faces[i] = {
            vertices[idx_face.indices[0]],
            vertices[idx_face.indices[1]],
            vertices[idx_face.indices[2]]
        };
    }

    return result;
}

void free_scene(scene_t* scene) {
    if (scene) {
        if (scene->faces) {
            free(scene->faces);
            scene->faces = 0;
        }
        if (scene->meshes) {
            free(scene->meshes);
            scene->meshes = 0;
        }
        scene->n_faces = 0;
        scene->n_meshes = 0;
        free(scene);
    }
}

