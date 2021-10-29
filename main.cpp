#include "image.h"
#include <cstdlib>
#include <utility>
#include <functional>
#include <algorithm>
#include "math.h"
#include "model.h"
#include <iostream>
using namespace std;

void line(Image& image, int x0, int y0, int x1, int y1, std::array<int, 3> color) { 
    bool steep = false;
    if (abs(x0-x1) < abs(y0-y1)) {
        swap(x0, y0);
        swap(x1, y1);
        steep = true;
    }

    if (x0 > x1) {
        swap(x0, x1);
        swap(y0, y1);
    }

    int y = y0;
    const int dy = abs(y1 - y0);
    const int dx = x1 - x0;
    int d = -2 * dy + dx;
    for (int x = x0; x <= x1; x++) {
        // cout << x << " " << y << " " << d << endl;
        if (steep) {
            image.get(y,x) = color;
        } else {
            image.get(x,y) = color;
        }
        
        if (d < 0) {
            y += (y1 > y0? 1 : -1);
            d += 2 * dx + -2 * dy;
        } else {
            d += -2 * dy;
        }
    }
    
}

void triangle(Image& image, vec4f v0, vec4f v1, vec4f v2, vec2f uv0, vec2f uv1, vec2f uv2, Model m) { 
    float x0{v0[0]}, x1{v1[0]}, x2{v2[0]};
    float y0{v0[1]}, y1{v1[1]}, y2{v2[1]};
    float z0{v0[2]}, z1{v1[2]}, z2{v2[2]};
    //line(image, x1, y1, x2, y2, color);
    //line(image, x1, y1, x0, y0, color);
    //line(image, x0, y0, x2, y2, color);
    function<float(float,float)> f01 = [=](float x, float y) {
        return (y0-y1)*x + (x1-x0)*y+x0*y1 - x1*y0;
    };
    function<float(float,float)> f12 = [=](float x, float y) {
        return (y1-y2)*x + (x2-x1)*y+x1*y2 - x2*y1;
    };
    function<float(float,float)> f20 = [=](float x, float y) {
        return (y2-y0)*x + (x0-x2)*y+x2*y0 - x0*y2;
    };

    const int x_min = ceil(min(x0, min(x1, x2)));
    const int x_max = floor(max(x0, max(x1, x2)));
    const int y_min = ceil(min(y0, min(y1, y2)));
    const int y_max = ceil(max(y0, max(y1, y2)));
    const float f_alpha = f12(x0, y0);
    const float f_beta = f20(x1, y1);
    const float f_gamma = f01(x2, y2);
    for (int x = x_min; x <= x_max; x++) {
        for (int y = y_min; y <= y_max; y++) {
            float alpha = f12(x, y) / f_alpha;
            float beta = f20(x, y) / f_beta;
            float gamma = f01(x, y) / f_gamma;
            if (alpha >= 0 and beta >= 0 and gamma >= 0) {
                if ((alpha > 0 or f_alpha * f12(-1, -1) > 0) and
                    (beta > 0 or f_beta * f20(-1, -1) > 0) and 
                    (gamma > 0 or f_gamma * f01(-1, -1) > 0)) {
                        float z = z0 * alpha + z1 * beta + z2* gamma;
                        if (image.get_zbuff(x, y) < z ) {
                            image.get_zbuff(x, y) = z;
                            vec2f dest = uv0 * alpha + uv1 * beta + uv2 * gamma;
                            image.get(x, y) = m.diffuse.get_f(dest[0], 1-dest[1]);
                        }
                        
                    }
            }
        }
    }
}

mat4f look_at(vec3f eye, vec3f center, vec3f up) {
    vec3f z = (eye-center).normalize();
    vec3f x = cross<3, float>(up, z).normalize();
    vec3f y = cross<3, float>(z, x).normalize();
    mat4f Minv = identity<4, float>();
    mat4f Tr = identity<4, float>();
    for (int i = 0; i < 3; i++) {
        Minv[0][i] = x[i];
        Minv[1][i] = y[i];
        Minv[2][i] = z[i];
        Tr[i][3] = -center[i];
    }
    return Minv*Tr;
}

mat4f viewport(int x, int y, int w, int h) {
    mat4f rv{};
    float depth = 1.0;
    rv[0][0] = float(w) / 2;
    rv[1][1] = float(h) / 2;
    rv[2][2] = depth / 2;
    rv[3][3] = 1.;
    rv[0][3] = float(x) + float(w) / 2;
    rv[1][3] = float(y) + float(h) / 2;
    rv[2][3] = depth / 2;
    return rv;
}

mat4f projection(float t, float b, float n, float f, float l, float r) {
    mat4f scale{};
    mat4f translate = identity<4, float>();
    scale[0][0] = 2 / (r - l);
    scale[1][1] = 2 / (t - b);
    scale[2][2] = 2 / (r - l);
    scale[3][3] = 1.;

    translate[0][3] = - (l + r) / 2;
    translate[1][3] = - (b + t) / 2;
    translate[2][3] = - (f + n) / 2;
    return scale * translate;
}

int main() {
    Image i {1024, 1024};

    Model m {"african_head.obj"};
    m.load_diffuse("african_head_diffuse.ppm");
    mat4f view = look_at({2,2,-2}, {0,0,0}, {0,1,0});
    mat4f proj = projection(1, -1, 1, -1, -1, 1);
    mat4f port = viewport(0,0, 1024, 1024);
    mat4f transformation = port * proj * view;
    for (int idx = 0; idx < m.faces_vrt.size(); idx++) {
        const auto& vrt {m.faces_vrt[idx]};
        const auto& tex {m.faces_tex[idx]};
        vec4f v1 = transformation * m.vertices[vrt[0]].to_homo(1);
        vec4f v2 = transformation * m.vertices[vrt[1]].to_homo(1);
        vec4f v3 = transformation * m.vertices[vrt[2]].to_homo(1);
        triangle(i,v1, v2, v3,
        m.uv[tex[0]], m.uv[tex[1]], m.uv[tex[2]], m);
    }
    i.write_to("output.ppm", true);
}
