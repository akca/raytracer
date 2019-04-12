#ifndef RAYTRACER_CAMERA_H
#define RAYTRACER_CAMERA_H

#include "vector3d.h"
#include "utility.h"
#include "ray.h"
#include <utility>
#include <random>

class Camera {
public:
    Vector3D position;
    Vector3D up;
    float near_distance;
    float focus_distance = 1;
    float aperture;
    int fov_y = 1;
    int num_samples = 1;
    std::string image_name;
    int image_width;
    int image_height;
    std::mt19937 mt;
    std::uniform_real_distribution<float> dist;

    virtual void createRay(Vec2f pixel_position, Ray &ray) = 0;

    Camera() = default;

    Camera(
            const Vector3D &cam_pos,
            const Vector3D &up_vector,
            float near_distance,
            int fov_y,
            int num_samples,
            std::string image_name,
            int image_width,
            int image_height) : position(cam_pos),
                                up(up_vector),
                                near_distance(near_distance),
                                fov_y(fov_y),
                                num_samples(num_samples),
                                image_name(std::move(image_name)),
                                image_width(image_width),
                                image_height(image_height),
                                mt((std::random_device()) ()),
                                dist(0.0f, 1.0f) {


    };

};

class LookAtCamera : public Camera {
public:
    Vector3D gaze_point;

    LookAtCamera(
            const Vector3D &cam_pos,
            const Vector3D &up_vector,
            float near_distance,
            int fov_y,
            int num_samples,
            const std::string &image_name,
            int image_width,
            int image_height,
            const Vector3D &gaze_point) : Camera(cam_pos, up_vector, near_distance, fov_y, num_samples, image_name,
                                                 image_width, image_height), gaze_point(gaze_point) {};

    void createRay(Vec2f pixel_position, Ray &ray) override {

    }
};

class StdCamera : public Camera {
public:
    Vec4f near_plane;
    Vector3D plane_start_point;
    Vector3D right;
    Vector3D gaze;
    float nearPlaneWidth;
    float nearPlaneHeight;

    StdCamera(
            const Vector3D &cam_pos,
            const Vector3D &up_vector,
            float near_distance,
            int fov_y,
            int num_samples,
            const std::string &image_name,
            int image_width,
            int image_height,
            Vec4f near_plane,
            const Vector3D &plane_start_point,
            const Vector3D &gaze) : Camera(cam_pos, up_vector, near_distance, fov_y, num_samples, image_name,
                                           image_width, image_height), near_plane(near_plane),
                                    plane_start_point(plane_start_point),
                                    gaze(gaze) {
        right = (gaze * up).normalize();
        up = (right * gaze).normalize();

        Vector3D centerOfPlane = position + gaze * near_distance;
        this->plane_start_point = centerOfPlane + (right * near_plane.x) + (up * near_plane.w);

        nearPlaneWidth = near_plane.y - near_plane.x;
        nearPlaneHeight = near_plane.w - near_plane.z;

    };

    // TODO optimize
    void createRay(Vec2f pixel_position, Ray &ray) override {

        pixel_position.x *= nearPlaneWidth;
        pixel_position.y *= nearPlaneHeight;

        Vector3D pixelPosition = plane_start_point +
                                 (right * pixel_position.x) -
                                 (up * pixel_position.y);

        Vector3D direction = pixelPosition - position;
        direction.normalize();

        Vector3D random_in_unit_circle = Vector3D(dist(mt) * 2 - 1, dist(mt) * 2 - 1, 0);

        Vector3D offset = random_in_unit_circle * aperture / 2;

        direction = direction * focus_distance;

        direction = direction - offset;

        direction.normalize();

        ray = Ray(position + offset, direction);
    }
};

#endif //RAYTRACER_CAMERA_H
