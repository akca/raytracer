#include "../include/parser.h"
#include "../include/sphere.h"
#include "../include/tinyxml2.h"
#include "../include/tinyply.h"
#include <sstream>
#include <fstream>

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

        Vector3D position;
        Vector3D up;
        float near_distance;
        float focus_distance = 1;
        float aperture = 0;
        int fov_y = 1;
        int num_samples = 1;
        std::string image_name;
        int image_width;
        int image_height;
        Vec4f near_plane;
        Vector3D gaze;
        Vector3D gaze_point;

        auto child = element->FirstChildElement("Position");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("Up");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("NearDistance");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("ImageResolution");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("ImageName");
        stream << child->GetText() << std::endl;

        stream >> position.e[0] >> position.e[1] >> position.e[2];
        stream >> up.e[0] >> up.e[1] >> up.e[2];
        stream >> near_distance;
        stream >> image_width >> image_height;
        stream >> image_name;

        child = element->FirstChildElement("NumSamples");
        if (child) {
            stream << child->GetText() << std::endl;
            stream >> num_samples;
        }

        child = element->FirstChildElement("FovY");
        if (child) {
            stream << child->GetText() << std::endl;
            stream >> fov_y;
        }

        child = element->FirstChildElement("FocusDistance");
        if (child) {
            stream << child->GetText() << std::endl;
            stream >> focus_distance;
        }

        child = element->FirstChildElement("ApertureSize");
        if (child) {
            stream << child->GetText() << std::endl;
            stream >> aperture;
        }


        if (element->Attribute("type") && std::strcmp(element->Attribute("type"), "lookAt") == 0) {
            child = element->FirstChildElement("GazePoint");
            stream << child->GetText() << std::endl;
            stream >> gaze_point.e[0] >> gaze_point.e[1] >> gaze_point.e[2];
            auto *camera = new LookAtCamera(position, up, near_distance, focus_distance, aperture, num_samples,
                                            image_name, image_width, image_height, fov_y, gaze_point);

            cameras.push_back(camera);

        } else {
            child = element->FirstChildElement("Gaze");
            stream << child->GetText() << std::endl;
            stream >> gaze.e[0] >> gaze.e[1] >> gaze.e[2];
            gaze.normalize();

            child = element->FirstChildElement("NearPlane");
            stream << child->GetText() << std::endl;
            stream >> near_plane.x >> near_plane.y >> near_plane.z >> near_plane.w;

            auto *camera = new StdCamera(position, up, near_distance, focus_distance, aperture, num_samples, image_name,
                                         image_width, image_height, near_plane, gaze);

            cameras.push_back(camera);
        }


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

            textures.emplace_back(name, intpol, decal, app);
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
        Vertex vertex;
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

        Mesh *new_mesh = new Mesh();
        const char *const shading_mode = element->Attribute("shadingMode");

        bool is_smooth_shading = shading_mode && strcmp(shading_mode, "smooth") == 0;

        child = element->FirstChildElement("Material");
        stream << child->GetText() << std::endl;
        stream >> new_mesh->material_id;

        // convert to zero index
        new_mesh->material_id--;

        child = element->FirstChildElement("Texture");
        if (child) {
            stream << child->GetText() << std::endl;
            stream >> new_mesh->texture_id;

            // convert to zero index
            new_mesh->texture_id--;

            textures[new_mesh->texture_id].loadImage();
        }

        child = element->FirstChildElement("Transformations");

        float *transformMatrix = nullptr;
        if (child) {
            transformations = child->GetText();
            transformMatrix = createTransformMatrix(
                    t_translation, t_rotation, t_scaling, false, transformations);

        } else {
            transformations = "";
        }

        child = element->FirstChildElement("Faces");
        const char *const ply_file = child->Attribute("plyFile");
        std::vector<ParserTriangle *> parser_triangles;


        if (ply_file != 0) {

            parsePly(ply_file, new_mesh->material_id, new_mesh->faces, is_smooth_shading);

        } else {

            int v0_id, v1_id, v2_id;

            stream << child->GetText() << std::endl;

            while (!(stream >> v0_id).eof()) {
                stream >> v1_id >> v2_id;
                Vec2f texCoord1, texCoord2, texCoord3;

                if (new_mesh->texture_id != -1) {
                    // texcoords
                    texCoord1 = {texCoordData[v0_id - 1].x, texCoordData[v0_id - 1].y};
                    texCoord2 = {texCoordData[v1_id - 1].x, texCoordData[v1_id - 1].y};
                    texCoord3 = {texCoordData[v2_id - 1].x, texCoordData[v2_id - 1].y};
                }

                Vertex &v1 = vertex_data[v0_id - 1];
                Vertex &v2 = vertex_data[v1_id - 1];
                Vertex &v3 = vertex_data[v2_id - 1];

                // apply transformations if there exists
                if (transformMatrix) {

                    v1.applyTransform(transformMatrix, false);
                    v2.applyTransform(transformMatrix, false);
                    v3.applyTransform(transformMatrix, false);
                }
                delete[] transformMatrix;
                transformMatrix = nullptr;

                if (is_smooth_shading) {
                    auto *new_triangle = new ParserTriangle(v1, v2, v3);

                    v1.addNormal(new_triangle->normal);
                    v2.addNormal(new_triangle->normal);
                    v3.addNormal(new_triangle->normal);

                    parser_triangles.push_back(new_triangle);

                } else {
                    auto *new_triangle = new Triangle(v1, v2, v3, new_mesh->material_id, new_mesh->texture_id,
                                                      texCoord1, texCoord2, texCoord3);
                    new_mesh->faces.push_back(new_triangle);
                }
            }

        }

        if (is_smooth_shading) {
            for (ParserTriangle *pt : parser_triangles) {

                // in order to use vertex normals (for smooth shading), we need to normalize them.
                (*pt).v1.normalizeNormal();
                (*pt).v2.normalizeNormal();
                (*pt).v3.normalizeNormal();

                // create real triangles from ParserTriangle's
                auto *new_triangle = new Triangle((*pt).v1, (*pt).v2, (*pt).v3, new_mesh->material_id,
                                                  new_mesh->texture_id,
                                                  Vec2f(0, 0), Vec2f(0, 0), Vec2f(0, 0));

                new_triangle->is_smooth_shading = true;

                new_triangle->vertex_normal_1 = (*pt).v1.normal;
                new_triangle->vertex_normal_2 = (*pt).v2.normal;
                new_triangle->vertex_normal_3 = (*pt).v3.normal;

                new_mesh->faces.push_back(new_triangle);
            }
        }

        meshes.push_back(new_mesh);

        // construct bvh for this mesh
        BVH *mesh_bvh = new BVH((Object **) new_mesh->faces.data(), new_mesh->faces.size());
        objects.push_back(mesh_bvh);

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

        float *transformMatrix = nullptr;

        if (child) {
            transformations = child->GetText();
            transformMatrix = createTransformMatrix(
                    t_translation, t_rotation, t_scaling, false, transformations);
        } else {
            transformations = "";
        }

        for (auto &face : meshes[baseMeshId]->faces) {

            //TODO instance calculations are wrong!

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
            auto *new_triangle = new Triangle(v1, edge1, edge2, material_id - 1, texture_id - 1,
                                              (*face).texCoord1, (*face).texCoord2, (*face).texCoord3);

            objects.push_back(new_triangle);
        }
        if (transformMatrix) {
            delete[] transformMatrix;
        }
        element = element->NextSiblingElement("MeshInstance");
    }

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

        auto *new_triangle = new Triangle(vertex_data[v0_id - 1], vertex_data[v1_id - 1], vertex_data[v2_id - 1],
                                          material_id - 1, texture_id - 1, texCoord1, texCoord2, texCoord3);

        // apply transformations if there exists
        if (!transformations.empty()) {
            float *transformMatrix = createTransformMatrix(
                    t_translation, t_rotation, t_scaling, false, transformations);

            new_triangle->edge1 = new_triangle->edge1 + new_triangle->v1;
            new_triangle->edge2 = new_triangle->edge2 + new_triangle->v1;
            new_triangle->v1.applyTransform(transformMatrix, false);
            new_triangle->edge1.applyTransform(transformMatrix, false);
            new_triangle->edge2.applyTransform(transformMatrix, false);
            new_triangle->edge1 = new_triangle->edge1 - new_triangle->v1;
            new_triangle->edge2 = new_triangle->edge2 - new_triangle->v1;
            new_triangle->normal = (new_triangle->edge1 * new_triangle->edge2).normalize();

            delete[] transformMatrix;
        }

        objects.push_back(new_triangle);

        element = element->NextSiblingElement("Triangle");
    }

    // Get Spheres
    element = root->FirstChildElement("Objects");
    element = element->FirstChildElement("Sphere");
    while (element) {
        int material_id;
        int texture_id = -1;
        int center_vertex_id;
        float radius;

        child = element->FirstChildElement("Material");
        stream << child->GetText() << std::endl;
        stream >> material_id;

        // convert to zero index
        material_id--;

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

            // convert to zero index
            texture_id--;

            textures[texture_id].loadImage();
        }
        child = element->FirstChildElement("Transformations");
        if (child) {
            transformations = child->GetText();
        } else {
            transformations = "";
        }

        auto *new_sph = new Sphere(vertex_data[center_vertex_id - 1], radius, material_id, texture_id);

        if (!transformations.empty()) {
            new_sph->transformMatrix = createTransformMatrix(
                    t_translation, t_rotation, t_scaling, false, transformations);
            new_sph->invTransformMatrix = createTransformMatrix(
                    t_translation, t_rotation, t_scaling, true, transformations);

            for (int x = 0; x < 4; x++) {
                for (int y = 0; y < 4; y++) {
                    new_sph->invTransposeTransformMatrix[x + 4 * y] =
                            new_sph->invTransformMatrix[y + 4 * x];
                }
            }
        }

        objects.push_back(new_sph);
        element = element->NextSiblingElement("Sphere");
    }

    // create root BVH
    root_bvh = new BVH(objects.data(), objects.size());
}
