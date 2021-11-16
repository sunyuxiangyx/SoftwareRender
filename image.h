#pragma once
#include <string>
#include <vector>
#include <array>
#include <memory>
#include "math.h"
class Image {
    int height, width, channel;
    int max_pixel_value = 255;
    std::unique_ptr<unsigned char[]> values;
    public:
    Image();
    Image(std::string filename);
    Image(int width, int height, int channel);
    
    vec3f get(int i, int j);
    void set(int i, int j, vec3f color);
    vec3f uv(float i, float j);
    // std::array<int, 3>& get_f(float i, float j);
    // float& get_zbuff(int i, int j);
    void write_png(std::string filename, bool flip_vert = false);
    void apply_gamma(float gamma);
    unsigned int get_height() const;
    unsigned int get_width() const;
    unsigned int get_channel() const;
};
