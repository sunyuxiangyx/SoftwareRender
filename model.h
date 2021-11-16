#include <vector>
#include "math.h"
#include "image.h"
#include <string>

struct Model {
    std::vector<vec3f> vertices;
    std::vector<vec2f> uv;
    std::vector<vec3f> normals;
    std::vector<vec3i> faces_vrt;
    std::vector<vec3i> faces_tex;
    std::vector<vec3i> faces_norm;
    public:
    Image diffuse, normal, specular;
    Model(const std::string filename);
    ~Model();
};

