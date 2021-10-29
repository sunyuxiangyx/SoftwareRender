#include "image.h"
#include <fstream>
#include <cmath>
#include <limits>
Image::Image() {}
Image::Image(std::string filename) {
    std::ifstream file{filename};
    file >> encoding_format >> width >> height >> max_pixel_value;
    if (encoding_format != "P3") {
        throw "only work for P3";
    }
    values.resize(height * width);
    z_buffer.resize(height * width, std::numeric_limits<float>::min());
    for (unsigned int i = 0; i < width * height; i++) {
        int r, g, b;
        file >> r >>g >>b;
        values[i] = {r, g, b};
    } 
}
Image::Image(unsigned int width, unsigned int height): height{height}, width{width} {
    values.resize(height * width);
    z_buffer.resize(height * width, std::numeric_limits<float>::min());
}
std::array<int, 3>& Image::get(int i, int j) {
    return values[j * width + i];
}
std::array<int, 3>& Image::get_f(float i, float j) {
    return values[int(j * height)* width + int(i * width)];
}
float& Image::get_zbuff(int i, int j) {
    return z_buffer[j * width + i];
}
void Image::write_to(std::string filename, bool flip_vert) {
    std::ofstream file{filename};
    file << encoding_format << " " << width << " " << height << " " <<max_pixel_value<<std::endl;
    if (! flip_vert) {
        for (const auto& pixel: values) {
            file << pixel[0] << " " << pixel[1] << " " << pixel[2] << " ";
        } 
    } else {
        for (int i = height - 1; i >= 0; i--) {
            for (int j = 0; j < width; j++) {
                file << values[i * width + j][0] << " " << values[i * width + j][1] << " " << values[i * width + j][2] << " ";
            }
        }
    }
    
    file << std::endl;
}

void Image::apply_gamma(float gamma) {
    for (auto& pixel : values) {
        pixel[0] = std::pow((float) pixel[0] / max_pixel_value, gamma) * max_pixel_value;
        pixel[1] = std::pow((float) pixel[1] / max_pixel_value, gamma) * max_pixel_value;
        pixel[2] = std::pow((float) pixel[2] / max_pixel_value, gamma) * max_pixel_value;
    }
}

unsigned int Image::get_height() const {
    return height;
}
unsigned int Image::get_width() const {
    return width;
}

