#pragma once
#include "math.h"
#include "image.h"
template<class Uniform, class VS_IN, class VS_OUT>
struct Shader {
    virtual ~Shader() {}
    virtual vec4f vertex_process(int trangle_idx, VS_IN vs_in) = 0;
    virtual vec3f fragment_process() = 0;
    virtual void fragment_interpolation(vec3f barycent) = 0;

    Uniform uniform;
    VS_OUT vs_out[3];
    VS_OUT fs_in;
};

struct Uniform {
    mat4f transform_view;
    mat4f transform;
    mat4f transform_IT;
    mat4f transform_viewport;
    mat4f transform_shadow;
    Image* shadow = nullptr;
    Image* diffuse = nullptr;
    Image* specular = nullptr;
    Image* normal = nullptr;
    vec3f light_pos;
    vec3f view_pos;
    vec3f light_dir;
};
struct VS_IN {
    vec4f vertex_position;
    vec2f uv_coord;
    vec3f normal;
    vec3f tangent;
    vec3f bitangent;
};
struct VS_OUT {
    vec2f uv_coord;
    vec3f normal;
    vec4f view_pos;
    vec4f obj_pos;
    vec4f viewport_pos;
};

struct PhongShader: public Shader<Uniform, class VS_IN, class VS_OUT> {
    vec4f vertex_process(int trangle_idx, VS_IN vs_in) override {
        vs_out[trangle_idx].uv_coord = vs_in.uv_coord;
        vs_out[trangle_idx].normal = vs_in.normal;
        vs_out[trangle_idx].obj_pos = vs_in.vertex_position;
        vs_out[trangle_idx].view_pos = uniform.transform_view * vs_in.vertex_position;
        vs_out[trangle_idx].viewport_pos = uniform.transform_viewport * vs_in.vertex_position;
        return vs_out[trangle_idx].viewport_pos;
    };

    vec3f fragment_process() override {
        float shadow = 0.3;
        if (uniform.shadow) {
            vec4f pos_shadow = uniform.transform_shadow * fs_in.viewport_pos;
            pos_shadow = pos_shadow/pos_shadow[3];
            float z = uniform.shadow -> get(pos_shadow[0], pos_shadow[1])[0];
            shadow += 0.7*(z < pos_shadow[2]+0.05);
            //std::cerr << shadow << std::endl;
        }
        vec3f kd = uniform.diffuse -> uv(fs_in.uv_coord[0], 1 - fs_in.uv_coord[1]);
        vec3f ks =  uniform.specular -> uv(fs_in.uv_coord[0], 1 - fs_in.uv_coord[1]);
        // vec3f n = uniform.normal -> uv(fs_in.uv_coord[0], 1 - fs_in.uv_coord[1]);
        vec3f n = fs_in.normal.normalize();

        vec3f e1 = (vs_out[1].obj_pos - vs_out[0].obj_pos).reduce_dim();
        vec3f e2 = (vs_out[2].obj_pos - vs_out[0].obj_pos).reduce_dim();
        float x1 = vs_out[1].uv_coord[0] - vs_out[0].uv_coord[0];
        float y1 = vs_out[1].uv_coord[1] - vs_out[0].uv_coord[1];
        float x2 = vs_out[2].uv_coord[0] - vs_out[0].uv_coord[0];
        float y2 = vs_out[2].uv_coord[1] - vs_out[0].uv_coord[1];
        float r = 1/(x1*y2 - x2*y1);
        vec3f t = (e1 * y2 - e2 * y1) * r;
        vec3f b = (e2 * x1 - e1 * x2) * r;

    
        t = (t - n * dot(t, n)).normalize();
        b = (b - n * dot(b, n) - t * dot(b, t)).normalize();
        mat3f tbn;
        tbn.set_col(0, t);
        tbn.set_col(1, b);
        tbn.set_col(2, n);
        n = tbn * ((uniform.normal -> uv(fs_in.uv_coord[0], 1 - fs_in.uv_coord[1])) * 2 + -1.f);
        n = (uniform.transform_IT * n.increment_dim(0)).reduce_dim().normalize() * -1;
        vec3f l = (uniform.transform_IT * uniform.light_dir.increment_dim(0)).reduce_dim().normalize();
        vec3f r2 = ((n * dot(n, l)) * 2 - l).normalize();
        float spec = std::pow(std::max(r2[2], 0.f), ks[0]);
        float diff = std::max(0.f, dot(n , l));
        vec3f color;
        for (int i = 0; i < 3; i++) {
            color[i] = std::min<float>(kd[i]* shadow * (0.1+ diff + .1* spec), 1.0);
        }
        //return kd;
        return color;
    };

    void fragment_interpolation(vec3f barycent) override {
        float z[3] = {vs_out[0].view_pos[2],vs_out[1].view_pos[2],vs_out[2].view_pos[2]};
        float z_p = 1. / (barycent[0]/z[0] +  barycent[1]/z[1] + barycent[02]/z[2]);

        fs_in = VS_OUT{};
        for (int i = 0; i < 3; i++) {
            fs_in.uv_coord = fs_in.uv_coord + vs_out[i].uv_coord * barycent[i] / z[i];
            fs_in.normal = fs_in.normal + vs_out[i].normal * barycent[i] / z[i];
            fs_in.viewport_pos = fs_in.viewport_pos + vs_out[i].viewport_pos * barycent[i] / z[i];
        }
        fs_in.uv_coord = fs_in.uv_coord * z_p;
        fs_in.normal = fs_in.normal * z_p;
        fs_in.viewport_pos = fs_in.viewport_pos * z_p;
        float u = fs_in.uv_coord[0];
        float v = fs_in.uv_coord[1];
        if (u < 0 or v < 0 or u > 1 or v > 1) {
            std::cerr << barycent << std::endl;
        }
    }
};

struct DepthShader: public Shader<Uniform, class VS_IN, class VS_OUT> { 
vec4f vertex_process(int trangle_idx, VS_IN vs_in) override {
        vs_out[trangle_idx].viewport_pos =  uniform.transform_viewport * vs_in.vertex_position;
        vs_out[trangle_idx].view_pos = uniform.transform_view * vs_in.vertex_position;
        return vs_out[trangle_idx].viewport_pos;
    };

    vec3f fragment_process() override {
        float z {fs_in.viewport_pos[2]};
        //z = (z + 1) / 2;
        return vec3f{z,z,z};
    };

    void fragment_interpolation(vec3f barycent) override {
        float z[3] = {vs_out[0].view_pos[2],vs_out[1].view_pos[2],vs_out[2].view_pos[2]};
        float z_p = 1. / (barycent[0]/z[0] +  barycent[1]/z[1] + barycent[02]/z[2]);

        fs_in = VS_OUT{};
        for (int i = 0; i < 3; i++) {
            fs_in.viewport_pos = fs_in.viewport_pos + vs_out[i].viewport_pos * barycent[i] / z[i];
        }
        fs_in.viewport_pos = fs_in.viewport_pos * z_p;
    }
};
