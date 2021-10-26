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

    Image diffuse, normal, specular;

    public:
    Model(const std::string filename);
    void load_diffuse(const std::string filename);
    ~Model();
};