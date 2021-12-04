#include "model.h"
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;
Model::Model(const std::string filename) {
    ifstream in {filename};
    string line;
    while(getline(in, line)) {
        istringstream iss{line};
        if (line.compare(0, 2, "v ") == 0) {
            iss.ignore();
            vec3f v;
            for (int i = 0; i < 3; i++) { iss >> v[i]; }
            vertices.push_back(v);
        } else if (line.compare(0, 3, "vn ") == 0) {
            iss.ignore(2);
            vec3f v;
            for (int i = 0; i < 3; i++) { iss >> v[i]; }
            normals.push_back(v);
        } else if (line.compare(0, 3, "vt ") == 0) {
            iss.ignore(2);
            vec2f v;
            for (int i = 0; i < 2; i++) { iss >> v[i]; }
            uv.push_back(v);
        } else if (line.compare(0, 2, "f ") == 0) {
            iss.ignore();
            vec3i vrt, tex, norm;
            bool tex_used = false, norm_used = false;
            for (int i = 0; i < 3; i++) {
                iss >> vrt[i];
                if (iss.peek() == '/') {
                    iss.ignore();
                    if (iss.peek() != '/') {
                        iss >> tex[i];
                        tex_used = true;
                    }
                }
                if (iss.peek() == '/') {
                    iss.ignore();
                    iss >> norm[i];
                    norm_used = true;
                }
            }
            faces_vrt.push_back(vrt + -1);
            if (tex_used) { faces_tex.push_back(tex + -1); }
            if (norm_used) { faces_norm.push_back(norm + -1); }
        }
    }
}

Model::~Model() {}
// void Model::load_diffuse(const std::string filename) {
//     diffuse = Image(filename);
// }