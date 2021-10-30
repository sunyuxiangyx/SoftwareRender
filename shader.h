#pragma once
#include "math.h"
#include "image.h"
template<class Uniform, class Varying>
struct Shader {
    virtual ~Shader() {}
    virtual vec4f vertex_process(vec4f v) = 0;
    virtual vec3i fragment_process(vec3f barycent) = 0;

    Uniform uniform;
    Varying varying;
};

struct Uniform {
    mat4f Transform;
    Image* diffuse = nullptr;
};
struct Varying {
    mat3f uv_coord;

};

struct PhongShader: public Shader<Uniform, Varying> {
    vec4f vertex_process(vec4f v) override {
        return uniform.Transform * v;
    };
    vec3i fragment_process(vec3f barycent) override {
        auto uv = varying.uv_coord * barycent;
        return uniform.diffuse -> get_f(uv[0], 1 - uv[1]);
    };
    ~PhongShader(){}
};
