#define TINYPLY_IMPLEMENTATION

#include <iostream>
#include <fstream>
#include "tinyply.h"
#include "parser.h"

using namespace tinyply;
using namespace parser;

void parsePly(const char *ply_path, int material, std::vector<Triangle *> &faces_vector, bool is_smooth_shading) {

    std::ifstream ss(ply_path, std::ios::binary);
    if (ss.fail()) throw std::runtime_error("failed to open ply file");

    PlyFile file;
    file.parse_header(ss);

    std::cout << "........................................................................\n";
    std::cout << "Parsing ply: " << ply_path << std::endl;
    for (auto c : file.get_comments()) std::cout << "Comment: " << c << std::endl;
    for (auto e : file.get_elements()) {
        std::cout << "element - " << e.name << " (" << e.size << ")" << std::endl;
        for (auto p : e.properties)
            std::cout << "\tproperty - " << p.name << " (" << tinyply::PropertyTable[p.propertyType].str << ")"
                      << std::endl;
    }
    std::cout << "........................................................................\n";

    std::shared_ptr<PlyData> vertices, normals, faces, texcoords;

    try {
        vertices = file.request_properties_from_element("vertex", {"x", "y", "z"});
    }
    catch (const std::exception &e) {
        std::cerr << "tinyply exception: " << e.what() << std::endl;
    }

    try {
        faces = file.request_properties_from_element("face", {"vertex_indices"}, 0);
    }
    catch (const std::exception &e) {
        std::cerr << "tinyply exception: " << e.what() << std::endl;
    }

    file.read(ss);

    if (vertices) std::cout << "\tRead " << vertices->count << " total vertices " << std::endl;
    if (normals) std::cout << "\tRead " << normals->count << " total vertex normals " << std::endl;
    if (texcoords) std::cout << "\tRead " << texcoords->count << " total vertex texcoords " << std::endl;
    if (faces) std::cout << "\tRead " << faces->count << " total faces (triangles) " << std::endl;


    const size_t numVerticesBytes = vertices->buffer.size_bytes();
    std::vector<Vec3f> verts(vertices->count);
    std::memcpy(verts.data(), vertices->buffer.get(), numVerticesBytes);

    const size_t numFacesBytes = faces->buffer.size_bytes();
    size_t list_size = numFacesBytes / sizeof(float) / faces->count;

    std::vector<Vertex> vertex_vector;
    std::vector<ParserTriangle *> parser_triangles;

    for (Vec3f &v : verts) {
        vertex_vector.emplace_back(v);
    }

    if (list_size == 3) {
        std::vector<Vec3i> faces_vec(faces->count);
        std::memcpy(faces_vec.data(), faces->buffer.get(), numFacesBytes);

        for (Vec3i &face : faces_vec) {
            Vertex &v1 = vertex_vector[face.x];
            Vertex &v2 = vertex_vector[face.y];
            Vertex &v3 = vertex_vector[face.z];

            if (is_smooth_shading) {
                auto *new_triangle = new ParserTriangle(v1, v2, v3);

                v1.addNormal(new_triangle->normal);
                v2.addNormal(new_triangle->normal);
                v3.addNormal(new_triangle->normal);

                parser_triangles.push_back(new_triangle);

            } else {
                Triangle *new_triangle = new Triangle(v1, v2, v3, material);
                faces_vector.push_back(new_triangle);
            }
        }

    } else if (list_size == 4) {
        std::vector<Vec4i> faces_vec(faces->count);
        std::memcpy(faces_vec.data(), faces->buffer.get(), numFacesBytes);

        for (Vec4i &face : faces_vec) {
            Vertex &v1 = vertex_vector[face.x];
            Vertex &v2 = vertex_vector[face.y];
            Vertex &v3 = vertex_vector[face.z];
            Vertex &v4 = vertex_vector[face.w];

            if (is_smooth_shading) {

                auto *new_triangle_1 = new ParserTriangle(v1, v2, v3);
                auto *new_triangle_2 = new ParserTriangle(v3, v4, v1);

                v1.addNormal(new_triangle_1->normal);
                v2.addNormal(new_triangle_1->normal);
                v3.addNormal(new_triangle_1->normal);
                v4.addNormal(new_triangle_1->normal);

                v1.addNormal(new_triangle_2->normal);
                v2.addNormal(new_triangle_2->normal);
                v3.addNormal(new_triangle_2->normal);
                v4.addNormal(new_triangle_2->normal);

                parser_triangles.push_back(new_triangle_1);
                parser_triangles.push_back(new_triangle_2);
            } else {
                Triangle *new_triangle_1 = new Triangle(v1, v2, v3, material);
                Triangle *new_triangle_2 = new Triangle(v3, v4, v1, material);

                faces_vector.push_back(new_triangle_1);
                faces_vector.push_back(new_triangle_2);
            }
        }

    } else {
        std::cout << "PLY vertex list with size " << list_size << " is not supported yet!" << std::endl;
    }

    if (is_smooth_shading) {

        for (ParserTriangle *pt : parser_triangles) {

            // in order to use vertex normals (for smooth shading), we need to normalize them.
            (*pt).v1.normalizeNormal();
            (*pt).v2.normalizeNormal();
            (*pt).v3.normalizeNormal();

            // create real triangles from ParserTriangle's
            auto *new_triangle = new Triangle((*pt).v1, (*pt).v2, (*pt).v3, material);

            new_triangle->is_smooth_shading = true;
            new_triangle->vertex_normal_1 = (*pt).v1.normal;
            new_triangle->vertex_normal_2 = (*pt).v2.normal;
            new_triangle->vertex_normal_3 = (*pt).v3.normal;

            faces_vector.push_back(new_triangle);
        }
    }
}

