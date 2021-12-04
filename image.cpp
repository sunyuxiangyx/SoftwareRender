#include "image.h"
#include <fstream>
#include <cmath>
#include <limits>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

Image::Image() {}
Image::Image(std::string filename) {
    unsigned char* temp = stbi_load(filename.data(), &width, &height, &channel, 0);
    if (temp != nullptr) {
        values = std::make_unique<float[]>(width * height * channel);
        for (int i = 0; i < width * height * channel; i++) {
            values[i] = temp[i]/ float(max_pixel_value);
        }
        stbi_image_free(temp);
    } else {
        throw "img not found/can't open";
    }
}

Image::Image(int width, int height, int channel) : height{height}, width{width}, channel{channel} {
    values = std::make_unique<float[]>(width * height * channel);
}

vec3f Image::get(int i, int j) {
    if (i < 0 or i >= width or j< 0 or j >= height) {
        std::cerr << i << " " << j <<std::endl; 
    }
    const auto idx {(j * width + i) * channel};
    return vec3f{float(values[idx]), float(values[idx+1]), float(values[idx+2])};
}


void Image::set(int i, int j, vec3f color) {
    const auto idx {(j * width + i) * channel};
    values[idx] = color[0];
    values[idx + 1] = color[1];
    values[idx + 2] = color[2];
}
vec3f Image::uv(float i, float j) {
    if (i < 0 or j < 0 or i > 1 or j > 1) {
        std::cerr << i << " " << j << std::endl;
    }
    const auto idx {(int(j * height) * width + int(i * width)) * channel};
    return vec3f{float(values[idx]), float(values[idx+1]), float(values[idx+2])};
}

void Image::write_png(std::string filename, bool flip_vert) {
    stbi_flip_vertically_on_write(flip_vert);
    auto temp = std::make_unique<unsigned char[]>(width * height * channel);
    for (int i = 0; i < width * height * channel; i++) {
        temp[i] = values[i] * max_pixel_value;
    }
    stbi_write_png(filename.data(), width, height, channel, temp.get(), width*channel);
}

void Image::apply_gamma(float gamma) {
    for (int i = 0; i < height * width * channel; i++) {
        values[i] = std::pow((float) values[i], gamma);
    }
}

unsigned int Image::get_height() const {
    return height;
}
unsigned int Image::get_width() const {
    return width;
}

