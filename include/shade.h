#ifndef RAYTRACER_SHADE_H
#define RAYTRACER_SHADE_H

#include "parser.h"
#include "object.h"
#include "camera.h"
#include "vector3d.h"

Vector3D reflect(const Vector3D &I, const Vector3D &N);

Vector3D refract(const Vector3D &I, const Vector3D &N, const float &refraction_idx);

void fresnel(const Vector3D &I, const Vector3D &N, const float &ior, float &kr);

Vector3D shade(parser::Scene &scene, Ray &ray, bool backfaceCulling, int recursionDepth);

void trace(parser::Scene *scene, Camera *camera, int startHeight,
           int endHeight, int imageWidth, int imageHeight,
           unsigned char *image);

#endif //RAYTRACER_SHADE_H
