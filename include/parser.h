#ifndef __RAYTRACER__PARSER__
#define __RAYTRACER__PARSER__

#include "camera.h"
#include "object.h"
#include "texture.h"
#include "triangle.h"
#include "utility.h"
#include "vector3d.h"
#include "vertex.h"
#include "bvh.h"
#include "mesh.h"
#include <string>
#include <vector>

namespace parser {

    struct PointLight {
        Vector3D position;
        Vector3D intensity;
    };

    struct Material {
        Vector3D ambient;
        Vector3D diffuse;
        Vector3D specular;
        Vector3D mirror;
        Vector3D transparency;
        float refraction_index = 1.f;
        float phong_exponent = 1.f;
    };

    struct Scene {
        // Data
        Vec3i background_color;
        float shadow_ray_epsilon;
        float intersection_test_epsilon;
        int max_recursion_depth;
        std::vector<Camera *> cameras;
        Vector3D ambient_light;
        std::vector<PointLight> point_lights;
        std::vector<Material> materials;
        std::vector<Texture> textures;
        std::vector<Vertex> vertex_data;
        std::vector<Vec2f> texCoordData;
        std::vector<Mesh *> meshes;
        std::vector<Object *> objects;
        std::vector<Vec3f> t_translation;
        std::vector<Vec4f> t_rotation;
        std::vector<Vec3f> t_scaling;
        BVH *root_bvh;

        // Functions
        void loadFromXml(const std::string &filepath);
    };

    /* this is used for calculating vertex normals */
    struct ParserTriangle {
        Vertex &v1;
        Vertex &v2;
        Vertex &v3;
        Vector3D normal;

        ParserTriangle(Vertex &a, Vertex &b, Vertex &c) : v1(a), v2(b), v3(c) {
            normal = ((b - a) * (c - a)).normalize();
        }
    };


} // namespace parser

void parsePly(const char *ply_path, int material, std::vector<Triangle *> &faces, float *transformMatrix,
              bool is_smooth_shading);

#endif
