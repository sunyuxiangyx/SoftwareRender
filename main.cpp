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
#include <vector>
using namespace std;

int main() {
    
    Model m {"african_head.obj"};
    m.diffuse = Image("african_head_diffuse.png");
    m.specular = Image("african_head_spec.png");
    m.normal = Image("african_head_nm_tangent.png");


    vec3f light_pos {0, 3, 3};
    vec3f light_dir {-2, 0, 0};
    vec3f view_pos {-1.5, 0, 1.5};
    unsigned int width {1024};
    unsigned int height {1024};
    mat4f light_transform_viewport;
    // shadow_map
    Image shadow {width, height, 3};
    {   
        DepthShader depth_shader;
        vector<double> z_buffer(shadow.get_width() * shadow.get_height(), std::numeric_limits<double>::min());
        mat4f view = look_at(light_pos, light_dir - light_pos, {0,1,0});
        mat4f proj = Mortho(4, -4, 0, 5, -4, 4);
        mat4f port = viewport(0,0, width, height);
        depth_shader.uniform.transform_viewport = light_transform_viewport= port * proj * view;
        for (int idx = 0; idx < m.faces_vrt.size(); idx++) {
            array<vec4f, 3> screen_coord_pts;
            VS_IN vs_in;
            const auto& vrt {m.faces_vrt[idx]};
            for (int i = 0; i < 3; i++) {
                vs_in.vertex_position = m.vertices[vrt[i]].increment_dim(1);
                screen_coord_pts[i] = depth_shader.vertex_process(i, vs_in);
            }
            triangle(shadow, depth_shader, screen_coord_pts, z_buffer);
        }
    }

    shadow.write_png("shadow.png", true);


    Image image {width, height, 3};

    PhongShader shader;
    
    vector<double> z_buffer(image.get_width() * image.get_height(), std::numeric_limits<double>::min());
   
    mat4f view = look_at(view_pos, {0,0,0}, {0,1,0});
    //mat4f proj = Mortho(1, -1, -1, 1, -1, 1);
    mat4f proj = Mpresp(90, float(image.get_width())/ image.get_height(), 0.3, 20);
    mat4f port = viewport(0,0, 1024, 1024);
    
    shader.uniform.transform_view = view;
    shader.uniform.transform = proj * view;
    shader.uniform.transform_IT = transpose(inverse(shader.uniform.transform));
    shader.uniform.transform_viewport = port * shader.uniform.transform;
    shader.uniform.transform_shadow = light_transform_viewport * inverse(shader.uniform.transform_viewport);
    shader.uniform.shadow = &shadow;
    shader.uniform.diffuse = &m.diffuse;
    shader.uniform.specular = &m.specular;
    shader.uniform.normal = &m.normal;
    shader.uniform.view_pos = view_pos;
    shader.uniform.light_pos = light_pos;
    shader.uniform.light_dir = light_dir;
    for (int idx = 0; idx < m.faces_vrt.size(); idx++) {
        array<vec4f, 3> screen_coord_pts;
        VS_IN vs_in;

        const auto& vrt {m.faces_vrt[idx]};
        const auto& tex {m.faces_tex[idx]};
        for (int i = 0; i < 3; i++) {
            vs_in.vertex_position = m.vertices[vrt[i]].increment_dim(1);
            vs_in.uv_coord = m.uv[tex[i]];
            vs_in.normal = m.normals[vrt[i]];
            screen_coord_pts[i] = shader.vertex_process(i, vs_in);
            screen_coord_pts[i] = screen_coord_pts[i] / screen_coord_pts[i][3];
        }
        
        triangle(image, shader, screen_coord_pts, z_buffer);
    }
    //image.apply_gamma(1/2.2);
    image.write_png("output.png", true);

}
