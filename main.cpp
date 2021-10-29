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

void triangle(Image& image, vec3i v0, vec3i v1, vec3i v2, vec2f uv0, vec2f uv1, vec2f uv2, Model m) { 
    int x0{v0[0]}, x1{v1[0]}, x2{v2[0]};
    int y0{v0[1]}, y1{v1[1]}, y2{v2[1]};
    int z0{v0[2]}, z1{v1[2]}, z2{v2[2]};
    //line(image, x1, y1, x2, y2, color);
    //line(image, x1, y1, x0, y0, color);
    //line(image, x0, y0, x2, y2, color);
    function<int(int,int)> f01 = [=](int x, int y) {
        return (y0-y1)*x + (x1-x0)*y+x0*y1 - x1*y0;
    };
    function<int(int,int)> f12 = [=](int x, int y) {
        return (y1-y2)*x + (x2-x1)*y+x1*y2 - x2*y1;
    };
    function<int(int,int)> f20 = [=](int x, int y) {
        return (y2-y0)*x + (x0-x2)*y+x2*y0 - x0*y2;
    };

    const int x_min = min(x0, min(x1, x2));
    const int x_max = max(x0, max(x1, x2));
    const int y_min = min(y0, min(y1, y2));
    const int y_max = max(y0, max(y1, y2));
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

int main() {
    Image i {1024, 1024};
    function<vec3i(vec4f)> f = [](vec4f x) {
        vec3i rv = {512 * (x[0] + 1), 512 * (x[1] + 1), 512 * (x[2] + 1) };
        //cout << int(512 * (x+1)) << endl;
        return rv;
    };
    Model m {"african_head.obj"};
    m.load_diffuse("african_head_diffuse.ppm");
    mat4f view = look_at({2,2,2}, {0,0,0}, {0,1,0});
    for (int idx = 0; idx < m.faces_vrt.size(); idx++) {
        const auto& vrt {m.faces_vrt[idx]};
        const auto& tex {m.faces_tex[idx]};

        vec3i v1 = f(view * m.vertices[vrt[0]].to_homo(1));
        vec3i v2 = f(view * m.vertices[vrt[1]].to_homo(1));
        vec3i v3 = f(view * m.vertices[vrt[2]].to_homo(1));
        triangle(i,v1, v2, v3,
        m.uv[tex[0]], m.uv[tex[1]], m.uv[tex[2]], m);
        //line(i, f(m.vertices[vrt[0]][0]), f(m.vertices[vrt[0]][1]), f(m.vertices[vrt[1]][0]), f(m.vertices[vrt[1]][1]),{255,255,255});
        //line(i, f(m.vertices[vrt[2]][0]), f(m.vertices[vrt[2]][1]), f(m.vertices[vrt[1]][0]), f(m.vertices[vrt[1]][1]),{255,255,255});
        //line(i, f(m.vertices[vrt[0]][0]), f(m.vertices[vrt[0]][1]), f(m.vertices[vrt[2]][0]), f(m.vertices[vrt[2]][1]),{255,255,255});
    }
    i.write_to("shit.ppm");
}
