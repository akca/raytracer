#include "../include/parser.h"
#include "../include/sphere.h"
#include "../include/texture.h"
#include "../include/tinyxml2.h"
#include "../include/triangle.h"
#include "../include/utility.h"
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <bvh.h>

void parser::Scene::loadFromXml(const std::string &filepath) {
    tinyxml2::XMLDocument file;
    std::stringstream stream;
    std::string transformations;

    auto res = file.LoadFile(filepath.c_str());
    if (res) {
        throw std::runtime_error("Error: The xml file cannot be loaded.");
    }

    auto root = file.FirstChild();
    if (!root) {
        std::stringstream stream;
        throw std::runtime_error("Error: Root is not found.");
    }

    // Get BackgroundColor
    auto element = root->FirstChildElement("BackgroundColor");
    if (element) {
        stream << element->GetText() << std::endl;
    } else {
        stream << "0 0 0" << std::endl;
    }
    stream >> background_color.x >> background_color.y >> background_color.z;

    // Get ShadowRayEpsilon
    element = root->FirstChildElement("ShadowRayEpsilon");
    if (element) {
        stream << element->GetText() << std::endl;
    } else {
        stream << "0.001" << std::endl;
    }
    stream >> shadow_ray_epsilon;

    // Get IntersectionTestEpsilon
    element = root->FirstChildElement("IntersectionTestEpsilon");
    if (element) {
        stream << element->GetText() << std::endl;
    } else {
        stream << "1e-7" << std::endl;
    }
    stream >> intersection_test_epsilon;


    // Get MaxRecursionDepth
    element = root->FirstChildElement("MaxRecursionDepth");
    if (element) {
        stream << element->GetText() << std::endl;
    } else {
        stream << "0" << std::endl;
    }
    stream >> max_recursion_depth;

    // Get Cameras
    element = root->FirstChildElement("Cameras");
    element = element->FirstChildElement("Camera");
    while (element) {

        Camera camera;

        auto child = element->FirstChildElement("Position");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("Gaze");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("Up");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("NearPlane");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("NearDistance");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("ImageResolution");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("ImageName");
        stream << child->GetText() << std::endl;

        stream >> camera.position.e[0] >> camera.position.e[1] >> camera.position.e[2];
        stream >> camera.gaze.e[0] >> camera.gaze.e[1] >> camera.gaze.e[2];
        stream >> camera.up.e[0] >> camera.up.e[1] >> camera.up.e[2];
        stream >> camera.near_plane.x >> camera.near_plane.y >>
               camera.near_plane.z >> camera.near_plane.w;
        stream >> camera.near_distance;
        stream >> camera.image_width >> camera.image_height;
        stream >> camera.image_name;

        camera.gaze.normalize();
        camera.right = (camera.gaze * camera.up).normalize();
        camera.up = camera.right * camera.gaze;
        camera.centerOfPlane = camera.position + camera.gaze * camera.near_distance;
        camera.planeStartPoint = camera.centerOfPlane +
                                 (camera.right * camera.near_plane.x) +
                                 (camera.up * camera.near_plane.w);
        cameras.push_back(camera);
        element = element->NextSiblingElement("Camera");
    }

    // Get Lights
    element = root->FirstChildElement("Lights");
    auto child = element->FirstChildElement("AmbientLight");
    stream << child->GetText() << std::endl;
    stream >> ambient_light.e[0] >> ambient_light.e[1] >> ambient_light.e[2];
    element = element->FirstChildElement("PointLight");

    while (element) {

        PointLight point_light;

        child = element->FirstChildElement("Position");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("Intensity");
        stream << child->GetText() << std::endl;

        stream >> point_light.position.e[0] >> point_light.position.e[1] >>
               point_light.position.e[2];
        stream >> point_light.intensity.e[0] >> point_light.intensity.e[1] >>
               point_light.intensity.e[2];

        point_lights.push_back(point_light);
        element = element->NextSiblingElement("PointLight");
    }

    // Get Materials
    element = root->FirstChildElement("Materials");
    element = element->FirstChildElement("Material");
    while (element) {

        Material material;

        child = element->FirstChildElement("AmbientReflectance");
        if (child) {
            stream << child->GetText() << std::endl;
            stream >> material.ambient.e[0] >> material.ambient.e[1] >> material.ambient.e[2];
        }
        child = element->FirstChildElement("DiffuseReflectance");
        if (child) {
            stream << child->GetText() << std::endl;
            stream >> material.diffuse.e[0] >> material.diffuse.e[1] >> material.diffuse.e[2];
        }
        child = element->FirstChildElement("SpecularReflectance");
        if (child) {
            stream << child->GetText() << std::endl;
            stream >> material.specular.e[0] >> material.specular.e[1] >>
                   material.specular.e[2];
        }
        child = element->FirstChildElement("MirrorReflectance");
        if (child) {
            stream << child->GetText() << std::endl;
            stream >> material.mirror.e[0] >> material.mirror.e[1] >> material.mirror.e[2];
        }
        child = element->FirstChildElement("PhongExponent");
        if (child) {
            stream << child->GetText() << std::endl;
            stream >> material.phong_exponent;
        }
        child = element->FirstChildElement("Transparency");
        if (child) {
            stream << child->GetText() << std::endl;
            stream >> material.transparency.e[0] >> material.transparency.e[1] >> material.transparency.e[2];
        }
        child = element->FirstChildElement("RefractionIndex");
        if (child) {
            stream << child->GetText() << std::endl;
            stream >> material.refraction_index;
        }

        materials.push_back(material);
        element = element->NextSiblingElement("Material");
    }

    // Get Textures
    element = root->FirstChildElement("Textures");
    if (element) {
        element = element->FirstChildElement("Texture");
        while (element) {
            std::string name, intpol, decal, app;

            child = element->FirstChildElement("ImageName");
            if (child) {
                stream << child->GetText() << std::endl;
                stream >> name;
            }
            child = element->FirstChildElement("Interpolation");
            if (child) {
                stream << child->GetText() << std::endl;
                stream >> intpol;
            }
            child = element->FirstChildElement("DecalMode");
            if (child) {
                stream << child->GetText() << std::endl;
                stream >> decal;
            }
            child = element->FirstChildElement("Appearance");
            if (child) {
                stream << child->GetText() << std::endl;
                stream >> app;
            }

            textures.push_back(Texture(name, intpol, decal, app));
            element = element->NextSiblingElement("Texture");
        }
    }

    // Get Transformations
    element = root->FirstChildElement("Transformations");
    if (element) {
        Vec3f tmp;
        child = element->FirstChildElement("Translation");
        while (child) {
            stream << child->GetText() << std::endl;
            stream >> tmp.x >> tmp.y >> tmp.z;
            t_translation.push_back(tmp);
            child = child->NextSiblingElement("Translation");
        }
        child = element->FirstChildElement("Scaling");
        while (child) {
            stream << child->GetText() << std::endl;
            stream >> tmp.x >> tmp.y >> tmp.z;
            t_scaling.push_back(tmp);
            child = child->NextSiblingElement("Scaling");
        }
        Vec4f tmp2;
        child = element->FirstChildElement("Rotation");
        while (child) {
            stream << child->GetText() << std::endl;
            stream >> tmp2.x >> tmp2.y >> tmp2.z >> tmp2.w;
            t_rotation.push_back(tmp2);
            child = child->NextSiblingElement("Rotation");
        }
    }

    // Get VertexData
    element = root->FirstChildElement("VertexData");
    if (element) {
        stream << element->GetText() << std::endl;
        Vector3D vertex;
        while (!(stream >> vertex.e[0]).eof()) {
            stream >> vertex.e[1] >> vertex.e[2];
            vertex_data.push_back(vertex);
        }
        stream.clear();
    }

    // Get TexCoordData
    element = root->FirstChildElement("TexCoordData");
    if (element) {
        stream << element->GetText() << std::endl;
        Vec2f coord;
        while (!(stream >> coord.x).eof()) {
            stream >> coord.y;
            texCoordData.push_back(coord);
        }
        stream.clear();
    }

    // Get Meshes
    element = root->FirstChildElement("Objects");
    element = element->FirstChildElement("Mesh");

    while (element) {
        int material_id;
        int texture_id = 0;

        child = element->FirstChildElement("Material");
        stream << child->GetText() << std::endl;
        stream >> material_id;

        child = element->FirstChildElement("Texture");
        if (child) {
            stream << child->GetText() << std::endl;
            stream >> texture_id;
            textures[texture_id - 1].loadImage();
        }

        child = element->FirstChildElement("Transformations");

        float *transformMatrix = NULL;
        if (child) {
            transformations = child->GetText();
            transformMatrix = createTransformMatrix(
                    t_translation, t_rotation, t_scaling, false, transformations);

        } else {
            transformations = "";
        }

        child = element->FirstChildElement("Faces");
        stream << child->GetText() << std::endl;

        int v0_id, v1_id, v2_id;
        std::vector<Triangle *> *faces = new std::vector<Triangle *>;

        while (!(stream >> v0_id).eof()) {
            stream >> v1_id >> v2_id;
            Vec2f texCoord1, texCoord2, texCoord3;

            if (texture_id != 0) {
                // texcoords
                texCoord1 = {texCoordData[v0_id - 1].x, texCoordData[v0_id - 1].y};
                texCoord2 = {texCoordData[v1_id - 1].x, texCoordData[v1_id - 1].y};
                texCoord3 = {texCoordData[v2_id - 1].x, texCoordData[v2_id - 1].y};
            }

            Vector3D v1 = vertex_data[v0_id - 1];
            Vector3D v2 = vertex_data[v1_id - 1];
            Vector3D v3 = vertex_data[v2_id - 1];

            // apply transformations if there exists
            if (transformMatrix) {

                v1.applyTransform(transformMatrix, false);
                v2.applyTransform(transformMatrix, false);
                v3.applyTransform(transformMatrix, false);
            }
            delete[] transformMatrix;
            transformMatrix = NULL;

            Triangle *newobj =
                    new Triangle(v1, v2 - v1, v3 - v1, material_id - 1, texture_id - 1,
                                 texCoord1, texCoord2, texCoord3);

            newobj->minPoint.e[0] = std::min(v1.x(), std::min(v2.x(), v3.x()));
            newobj->minPoint.e[1] = std::min(v1.y(), std::min(v2.y(), v3.y()));
            newobj->minPoint.e[2] = std::min(v1.z(), std::min(v2.z(), v3.z()));

            newobj->maxPoint.e[0] = std::max(v1.x(), std::max(v2.x(), v3.x()));
            newobj->maxPoint.e[1] = std::max(v1.y(), std::max(v2.y(), v3.y()));
            newobj->maxPoint.e[2] = std::max(v1.z(), std::max(v2.z(), v3.z()));


            faces->push_back(newobj);
            // mesh objects
            objects.push_back(newobj);
        }
        meshes.push_back(faces);

        stream.clear();
        element = element->NextSiblingElement("Mesh");
    }

    stream.clear();

    // Get MeshInstances
    element = root->FirstChildElement("Objects");
    element = element->FirstChildElement("MeshInstance");

    while (element) {
        int material_id;
        int texture_id = 0;
        int baseMeshId = std::stoi(element->Attribute("baseMeshId")) - 1;

        child = element->FirstChildElement("Material");
        stream << child->GetText() << std::endl;
        stream >> material_id;

        child = element->FirstChildElement("Texture");
        if (child) {
            stream << child->GetText() << std::endl;
            stream >> texture_id;
            textures[texture_id - 1].loadImage();
        }

        child = element->FirstChildElement("Transformations");

        float *transformMatrix = NULL;

        if (child) {
            transformations = child->GetText();
            transformMatrix = createTransformMatrix(
                    t_translation, t_rotation, t_scaling, false, transformations);
        } else {
            transformations = "";
        }

        for (auto &face : *meshes[baseMeshId]) {
            Vector3D v1 = (*face).v1;
            Vector3D edge1 = (*face).edge1;
            Vector3D edge2 = (*face).edge2;

            if (transformMatrix) {
                edge1 = edge1 + v1;
                edge2 = edge2 + v1;
                v1.applyTransform(transformMatrix, false);
                edge1.applyTransform(transformMatrix, false);
                edge2.applyTransform(transformMatrix, false);
                edge1 = edge1 - v1;
                edge2 = edge2 - v1;
            }
            Triangle *newobj =
                    new Triangle(v1, edge1, edge2, material_id - 1, texture_id - 1,
                                 (*face).texCoord1, (*face).texCoord2, (*face).texCoord3);

            newobj->minPoint.e[0] = std::min(v1.x(), std::min(edge1.x(), edge2.x()));
            newobj->minPoint.e[1] = std::min(v1.y(), std::min(edge1.y(), edge2.y()));
            newobj->minPoint.e[2] = std::min(v1.z(), std::min(edge1.z(), edge2.z()));

            newobj->maxPoint.e[0] = std::max(v1.x(), std::max(edge1.x(), edge2.x()));
            newobj->maxPoint.e[1] = std::max(v1.y(), std::max(edge1.y(), edge2.y()));
            newobj->maxPoint.e[2] = std::max(v1.z(), std::max(edge1.z(), edge2.z()));

            objects.push_back(newobj);
        }
        if (transformMatrix) {
            delete[] transformMatrix;
            transformMatrix = NULL;
        }
        element = element->NextSiblingElement("MeshInstance");
    }

    // deallocate mesh vectors
    for (auto &mesh : meshes) {
        delete mesh;
    }
    meshes.clear();

    stream.clear();

    // Get Triangles
    element = root->FirstChildElement("Objects");
    element = element->FirstChildElement("Triangle");

    while (element) {
        int material_id;
        int texture_id = 0;
        int v0_id, v1_id, v2_id;

        child = element->FirstChildElement("Material");
        stream << child->GetText() << std::endl;
        stream >> material_id;

        child = element->FirstChildElement("Indices");
        stream << child->GetText() << std::endl;
        stream >> v0_id >> v1_id >> v2_id;

        child = element->FirstChildElement("Texture");
        if (child) {
            stream << child->GetText() << std::endl;
            stream >> texture_id;
            textures[texture_id - 1].loadImage();
        }
        child = element->FirstChildElement("Transformations");
        if (child) {
            transformations = child->GetText();
        } else {
            transformations = "";
        }

        Vec2f texCoord1, texCoord2, texCoord3;

        if (texture_id != 0) {
            // texcoords
            texCoord1 = {texCoordData[v0_id - 1].x, texCoordData[v0_id - 1].y};
            texCoord2 = {texCoordData[v1_id - 1].x, texCoordData[v1_id - 1].y};
            texCoord3 = {texCoordData[v2_id - 1].x, texCoordData[v2_id - 1].y};
        }
        Triangle *newobj = new Triangle(
                vertex_data[v0_id - 1], vertex_data[v1_id - 1] - vertex_data[v0_id - 1],
                vertex_data[v2_id - 1] - vertex_data[v0_id - 1], material_id - 1,
                texture_id - 1, texCoord1, texCoord2, texCoord3);

        // apply transformations if there exists
        if (transformations != "") {
            float *transformMatrix = createTransformMatrix(
                    t_translation, t_rotation, t_scaling, false, transformations);

            newobj->edge1 = newobj->edge1 + newobj->v1;
            newobj->edge2 = newobj->edge2 + newobj->v1;
            newobj->v1.applyTransform(transformMatrix, false);
            newobj->edge1.applyTransform(transformMatrix, false);
            newobj->edge2.applyTransform(transformMatrix, false);
            newobj->edge1 = newobj->edge1 - newobj->v1;
            newobj->edge2 = newobj->edge2 - newobj->v1;
            newobj->normal = (newobj->edge1 * newobj->edge2).normalize();

            delete[] transformMatrix;
        }

        objects.push_back(newobj);

        element = element->NextSiblingElement("Triangle");
    }

    // Get Spheres
    element = root->FirstChildElement("Objects");
    element = element->FirstChildElement("Sphere");
    while (element) {
        int material_id;
        int texture_id = 0;
        int center_vertex_id;
        float radius;

        child = element->FirstChildElement("Material");
        stream << child->GetText() << std::endl;
        stream >> material_id;

        child = element->FirstChildElement("Center");
        stream << child->GetText() << std::endl;
        stream >> center_vertex_id;

        child = element->FirstChildElement("Radius");
        stream << child->GetText() << std::endl;
        stream >> radius;

        child = element->FirstChildElement("Texture");
        if (child) {
            stream << child->GetText() << std::endl;
            stream >> texture_id;
            textures[texture_id - 1].loadImage();
        }
        child = element->FirstChildElement("Transformations");
        if (child) {
            transformations = child->GetText();
        } else {
            transformations = "";
        }
        Sphere *newobj = new Sphere(vertex_data[center_vertex_id - 1], radius,
                                    material_id - 1, texture_id - 1);

        if (transformations != "") {
            newobj->transformMatrix = createTransformMatrix(
                    t_translation, t_rotation, t_scaling, false, transformations);
            newobj->invTransformMatrix = createTransformMatrix(
                    t_translation, t_rotation, t_scaling, true, transformations);

            for (int x = 0; x < 4; x++) {
                for (int y = 0; y < 4; y++) {
                    newobj->invTransposeTransformMatrix[x + 4 * y] =
                            newobj->invTransformMatrix[y + 4 * x];
                }
            }
        }

        objects.push_back(newobj);
        element = element->NextSiblingElement("Sphere");
    }

    BVH* root_bvh = new BVH(objects.data(), objects.size());

    std::vector<Object *> bvh_obj;
    bvh_obj.push_back(root_bvh);

    objects = bvh_obj;

}
