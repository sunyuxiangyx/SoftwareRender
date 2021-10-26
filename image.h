#pragma once
#include <string>
#include <vector>
#include <array>
class Image {
    std::string encoding_format = "P3";
    unsigned int height, width;
    unsigned int max_pixel_value = 255;
    std::vector<std::array<int, 3>> values;
    std::vector<float> z_buffer;
    bool inilized = false;
    public:
    Image(std::string filename);
    Image(unsigned int width, unsigned int height);
    Image();
    
    std::array<int, 3>& get(int i, int j);
    std::array<int, 3>& get_f(float i, float j);
    float& get_zbuff(int i, int j);
    void write_to(std::string filename);
    void apply_gamma(float gamma);
    unsigned int get_height() const;
    unsigned int get_width() const;
};
