#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

typedef float point_t;
struct vec3 {
    point_t x;
    point_t y;
    point_t z;
};

struct idx_face_t {
    unsigned int indices[3];
    unsigned int index;

    idx_face_t(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int index) {
        indices[0] = p1;
        indices[1] = p2;
        indices[2] = p3;
        index = index;
    }

    idx_face_t() {}
};

struct face_t
{
    vec3 points[3];
};

struct mesh_t
{
    int first_idx;
    int n_faces;
};

struct scene_t
{
    int n_faces;
    face_t *faces;
    int n_meshes;
    mesh_t* meshes;
};

#define SCENE_POSTPROCESS_NONE 0
#define SCENE_POSTPROCESS_SCALE_AND_MOVE 1
#define SCENE_POSTPROCESS_NORMALS 2

inline vec3 make_vec(point_t x, point_t y, point_t z) {
    return { x, y, z };
}

inline void operator -=(vec3& a, vec3 b) {
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
}

inline void operator /=(vec3& a, point_t b) {
    b = point_t(1) / b;
    a.x *= b;
    a.y *= b;
    a.z *= b;
}

scene_t* load_scene(const char *filePath, int post_process);
void free_scene(scene_t* scene);


