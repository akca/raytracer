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
    Vector3D right;
    Vector3D gaze;
    Vector3D plane_start_point;
    float nearPlaneWidth;
    float nearPlaneHeight;
    Vec4f near_plane;
    float near_distance;
    float focus_distance = 1;
    float lens_radius; // == aperture / 2
    int num_samples = 1;
    std::string image_name;
    int image_width;
    int image_height;
    std::mt19937 mt;
    std::uniform_real_distribution<float> dist;

    Camera() = default;

    Camera(
            const Vector3D &cam_pos,
            const Vector3D &up_vector,
            float near_distance,
            float focus_distance,
            float aperture,
            int num_samples,
            std::string image_name,
            int image_width,
            int image_height) : position(cam_pos),
                                up(up_vector),
                                near_distance(near_distance),
                                focus_distance(focus_distance),
                                lens_radius(aperture / 2),
                                num_samples(num_samples),
                                image_name(std::move(image_name)),
                                image_width(image_width),
                                image_height(image_height),
                                mt((std::random_device()) ()),
                                dist(0.0f, 1.0f) {

    };

    void createRay(Vec2f pixel_position, Ray &ray) {
        pixel_position.x *= nearPlaneWidth;
        pixel_position.y *= nearPlaneHeight;

        Vector3D pixelPosition = plane_start_point +
                                 (right * pixel_position.x) -
                                 (up * pixel_position.y);

        Vector3D direction = pixelPosition - position;
        direction.normalize();

        Vector3D random_in_unit_circle = Vector3D(dist(mt) * 2 - 1, dist(mt) * 2 - 1, 0);

        Vector3D offset = random_in_unit_circle * lens_radius;

        direction = direction * focus_distance;

        direction = direction - offset;

        direction.normalize();

        ray = Ray(position + offset, direction);
    }

};

class LookAtCamera : public Camera {
public:

    int fov_y;
    Vector3D gaze_point;

    LookAtCamera(
            const Vector3D &cam_pos,
            const Vector3D &up_vector,
            float near_distance,
            float focus_distance,
            float aperture,
            int num_samples,
            const std::string &image_name,
            int image_width,
            int image_height,
            int fov_y,
            const Vector3D &gaze_point) : Camera(cam_pos, up_vector, near_distance, focus_distance, aperture,
                                                 num_samples, image_name, image_width, image_height), fov_y(fov_y),
                                          gaze_point(gaze_point) {

        this->gaze = (gaze_point - cam_pos).normalize();
        up.normalize();

        right = (gaze * up).normalize();
        up = (right * gaze).normalize();

        Vector3D centerOfPlane = position + gaze * near_distance;

        // compute nearplane
        float verticalDist = std::tan((this->fov_y / 2.f) * (M_PI / 180.f)) * near_distance;
        float horizontalDist = verticalDist * ((float) image_width / image_height);

        this->near_plane.x = -horizontalDist; // left
        this->near_plane.y = horizontalDist;  // right
        this->near_plane.z = -verticalDist;   // bottom
        this->near_plane.w = verticalDist;    // top

        this->plane_start_point = centerOfPlane + (right * near_plane.x) + (up * near_plane.w);

        nearPlaneWidth = near_plane.y - near_plane.x;
        nearPlaneHeight = near_plane.w - near_plane.z;

    };
};

class StdCamera : public Camera {
public:

    StdCamera(
            const Vector3D &cam_pos,
            const Vector3D &up_vector,
            float near_distance,
            float focus_distance,
            float aperture,
            int num_samples,
            const std::string &image_name,
            int image_width,
            int image_height,
            Vec4f near_plane,
            const Vector3D &gaze) : Camera(cam_pos, up_vector, near_distance, focus_distance, aperture, num_samples,
                                           image_name, image_width, image_height) {
        this->gaze = gaze;
        this->near_plane = near_plane;

        right = (gaze * up).normalize();
        up = (right * gaze).normalize();

        Vector3D centerOfPlane = position + gaze * near_distance;
        this->plane_start_point = centerOfPlane + (right * near_plane.x) + (up * near_plane.w);

        nearPlaneWidth = near_plane.y - near_plane.x;
        nearPlaneHeight = near_plane.w - near_plane.z;

    };

};

#endif //RAYTRACER_CAMERA_H
