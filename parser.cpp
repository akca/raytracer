#include "face.h"
#include "mesh.h"
#include "parser.h"
#include "sphere.h"
#include "tinyxml2.h"
#include "triangle.h"
#include <sstream>
#include <stdexcept>

void parser::Scene::loadFromXml(const std::string &filepath) {
	tinyxml2::XMLDocument file;
	std::stringstream stream;

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
	Camera camera;
	while (element) {
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

		stream >> camera.position.x >> camera.position.y >> camera.position.z;
		stream >> camera.gaze.x >> camera.gaze.y >> camera.gaze.z;
		stream >> camera.up.x >> camera.up.y >> camera.up.z;
		stream >> camera.near_plane.x >> camera.near_plane.y >>
			camera.near_plane.z >> camera.near_plane.w;
		stream >> camera.near_distance;
		stream >> camera.image_width >> camera.image_height;
		stream >> camera.image_name;

		cameras.push_back(camera);
		element = element->NextSiblingElement("Camera");
	}

	// Get Lights
	element = root->FirstChildElement("Lights");
	auto child = element->FirstChildElement("AmbientLight");
	stream << child->GetText() << std::endl;
	stream >> ambient_light.x >> ambient_light.y >> ambient_light.z;
	element = element->FirstChildElement("PointLight");
	PointLight point_light;
	while (element) {
		child = element->FirstChildElement("Position");
		stream << child->GetText() << std::endl;
		child = element->FirstChildElement("Intensity");
		stream << child->GetText() << std::endl;

		stream >> point_light.position.x >> point_light.position.y >>
			point_light.position.z;
		stream >> point_light.intensity.x >> point_light.intensity.y >>
			point_light.intensity.z;

		point_lights.push_back(point_light);
		element = element->NextSiblingElement("PointLight");
	}

	// Get Materials
	element = root->FirstChildElement("Materials");
	element = element->FirstChildElement("Material");
	Material material;
	while (element) {
		child = element->FirstChildElement("AmbientReflectance");
		stream << child->GetText() << std::endl;
		child = element->FirstChildElement("DiffuseReflectance");
		stream << child->GetText() << std::endl;
		child = element->FirstChildElement("SpecularReflectance");
		stream << child->GetText() << std::endl;
		child = element->FirstChildElement("MirrorReflectance");
		stream << child->GetText() << std::endl;
		child = element->FirstChildElement("PhongExponent");
		stream << child->GetText() << std::endl;

		stream >> material.ambient.x >> material.ambient.y >> material.ambient.z;
		stream >> material.diffuse.x >> material.diffuse.y >> material.diffuse.z;
		stream >> material.specular.x >> material.specular.y >> material.specular.z;
		stream >> material.mirror.x >> material.mirror.y >> material.mirror.z;
		stream >> material.phong_exponent;

		materials.push_back(material);
		element = element->NextSiblingElement("Material");
	}

	// Get VertexData
	element = root->FirstChildElement("VertexData");
	stream << element->GetText() << std::endl;
	Vector3D vertex;
	while (!(stream >> vertex.x).eof()) {
		stream >> vertex.y >> vertex.z;
		vertex_data.push_back(vertex);
	}
	stream.clear();

	// Get Meshes
	element = root->FirstChildElement("Objects");
	element = element->FirstChildElement("Mesh");

	while (element) {
		std::vector<Face> faces;
		int material_id;

		child = element->FirstChildElement("Material");
		stream << child->GetText() << std::endl;
		stream >> material_id;

		child = element->FirstChildElement("Faces");
		stream << child->GetText() << std::endl;

		int v0_id, v1_id, v2_id;

		while (!(stream >> v0_id).eof()) {
			stream >> v1_id >> v2_id;

			Face face(vertex_data[v0_id - 1], vertex_data[v1_id - 1],
					vertex_data[v2_id - 1]);

			faces.push_back(face);
		}
		stream.clear();

		objects.push_back(new Mesh(faces, material_id - 1)); // TODO DEALLOCATION
		// mesh.faces.clear();
		element = element->NextSiblingElement("Mesh");
	}
	stream.clear();

	// Get Triangles
	element = root->FirstChildElement("Objects");
	element = element->FirstChildElement("Triangle");

	while (element) {
		int material_id;
		int v0_id, v1_id, v2_id;

		child = element->FirstChildElement("Material");
		stream << child->GetText() << std::endl;
		stream >> material_id;

		child = element->FirstChildElement("Indices");
		stream << child->GetText() << std::endl;
		stream >> v0_id >> v1_id >> v2_id;

		Face face(vertex_data[v0_id - 1], vertex_data[v1_id - 1],
				vertex_data[v2_id - 1]);

		objects.push_back(new Triangle(face, material_id - 1)); // TODO DEALLOCATION

		element = element->NextSiblingElement("Triangle");
	}

	// Get Spheres
	element = root->FirstChildElement("Objects");
	element = element->FirstChildElement("Sphere");
	while (element) {
		int material_id;
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

		objects.push_back(new Sphere(vertex_data[center_vertex_id - 1], radius,
					material_id - 1)); // TODO DEALLOCATION
		element = element->NextSiblingElement("Sphere");
	}
}
