#include "image.h"
#include <cstdlib>
#include <utility>
#include <functional>
#include <algorithm>
#include "math.h"
#include "model.h"
#include <iostream>
#include "shader.h"
#include "graphics.h"
using namespace std;

int main() {
    PhongShader shader;
    Image i {1024, 1024};

    Model m {"african_head.obj"};
    m.load_diffuse("african_head_diffuse.ppm");
    mat4f view = look_at({2,2,-2}, {0,0,0}, {0,1,0});
    mat4f proj = projection(1, -1, 1, -1, -1, 1);
    mat4f port = viewport(0,0, 1024, 1024);
    shader.uniform.Transform = port * proj * view;
    shader.uniform.diffuse = &m.diffuse;
    for (int idx = 0; idx < m.faces_vrt.size(); idx++) {
        array<vec4f, 3> screen_coord_pts;
        shader.varying.uv_coord = mat3f{};

        const auto& vrt {m.faces_vrt[idx]};
        const auto& tex {m.faces_tex[idx]};
        for (int i = 0; i < 3; i++) {
            screen_coord_pts[i] = shader.vertex_process(m.vertices[vrt[i]].increment_dim(1));
            shader.varying.uv_coord.set_col(i, m.uv[tex[i]].increment_dim(0));
        }
        
        triangle(i, shader, screen_coord_pts);
    }
    i.write_to("output.ppm", true);
}
