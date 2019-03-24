#include "../include/parser.h"
#include <iostream>
#include <thread>
#include <algorithm>

const float kRefractionBias = 0.0001f;

/**
 * Find the direction of perfect reflection.
 *
 * @param I incoming ray
 * @param N normal
 * @return reflection ray
 */
Vector3D reflect(const Vector3D &I, const Vector3D &N) {
    return (I - N * I.dotProduct(N) * 2).normalize();
}

/**
 * Compute refraction direction
 *
 * @param I incoming ray
 * @param N normal vector
 * @param refraction_idx object refraction index
 * @return refraction direction
 */
Vector3D refract(const Vector3D &I, const Vector3D &N, const float &refraction_idx) {

    float cosi = std::clamp(I.dotProduct(N), -1.0f, 1.0f);
    float ri_air = 1; // refractive index of the air
    float ri_obj = refraction_idx; // refractive index of the object
    Vector3D n = N;

    if (cosi < 0) {
        cosi = -cosi;
    } else {
        std::swap(ri_air, ri_obj);
        n = N.inverse();
    }

    float eta = ri_air / ri_obj;
    float k = 1 - eta * eta * (1 - cosi * cosi);

    return k < 0 ? Vector3D() : I * eta + n * (eta * cosi - sqrtf(k));
}

/**
 * Evaluate Fresnel equation (ratio of reflected light for a given incident direction and surface normal)
 *
 * @param I incoming ray
 * @param N normal
 * @param ior refractive index of the object
 * @param kr reflectance
 */
void fresnel(const Vector3D &I, const Vector3D &N, const float &ior, float &kr) {
    float cosi = std::clamp(I.dotProduct(N), -1.0f, 1.0f);
    float ri_air = 1; // refractive index of air
    float ri_obj = ior; // refractive index of the object

    if (cosi > 0) { std::swap(ri_air, ri_obj); }

    // compute sint using snell's law
    float sint = (ri_air / ri_obj) * sqrtf(std::max(0.f, 1 - cosi * cosi));

    // total internal reflection
    if (sint >= 1) {
        kr = 1;
    } else {
        float cost = sqrtf(std::max(0.f, 1 - sint * sint));
        cosi = fabsf(cosi);
        float Rs = ((ri_obj * cosi) - (ri_air * cost)) / ((ri_obj * cosi) + (ri_air * cost));
        float Rp = ((ri_air * cosi) - (ri_obj * cost)) / ((ri_air * cosi) + (ri_obj * cost));
        kr = (Rs * Rs + Rp * Rp) / 2;
    }
    // Note: As a consequence of the conservation of energy, transmittance is given by:
    // kt = 1 - kr;
}

Vector3D shade(parser::Scene &scene, Vector3D &rayOrigin, Vector3D &direction, bool backfaceCulling, int recursionDepth) {

    if (recursionDepth > scene.max_recursion_depth)
        return Vector3D(scene.background_color);

    Object *intersectObject = nullptr;
    float tmin = std::numeric_limits<float>::max();
    Vector3D normal, pixelColor, intersectPoint;
    Vec2f texCoord; // texture coord

    // search for nearest intersection
    for (auto &object : scene.objects) {
        if (object->intersects(Ray(rayOrigin, direction), tmin, intersectPoint, normal, backfaceCulling, texCoord)) {
            intersectObject = object;
        }
    }

    // if intersection with an object found
    if (intersectObject != nullptr) {

        intersectPoint = rayOrigin + direction * tmin;

        Vector3D &kDiffuse = scene.materials[intersectObject->material_id].diffuse;
        Vector3D &kAmbient = scene.materials[intersectObject->material_id].ambient;
        Vector3D &kSpecular = scene.materials[intersectObject->material_id].specular;
        Vector3D &kMirror = scene.materials[intersectObject->material_id].mirror;
        Vector3D &kTransparency = scene.materials[intersectObject->material_id].transparency;
        const float &kRefraction = scene.materials[intersectObject->material_id].refraction_index;

        // ambient light
        pixelColor = kAmbient.multiply(scene.ambient_light);

        for (auto &light : scene.point_lights) {

            Vector3D wi = light.position - intersectPoint;
            float lightDistance = intersectPoint.distance(light.position);
            wi = wi / lightDistance; // normalize

            Vector3D shadowRayOrigin = intersectPoint + wi * scene.shadow_ray_epsilon;

            // search objects between light and intersection point
            float stmin = lightDistance;
            bool underShadow = false;
            Vector3D tmp_vec;
            Vec2f tmp_vec2;

            // shadow ray
            for (auto &sobject : scene.objects) {
                if (sobject->intersects(Ray(shadowRayOrigin, wi), stmin, tmp_vec, tmp_vec, false, tmp_vec2)) {
                    underShadow = true;
                    break;
                }
            }

            if (!underShadow) {
                bool isReplaceAll = false;

                float costheta_diff = std::max(float(0), wi.dotProduct(normal));
                float distance2 = powf(lightDistance, 2);

                // diffuse shading
                if (intersectObject->texture_id == -1) {
                    pixelColor =
                            pixelColor +
                            (kDiffuse * costheta_diff).multiply(light.intensity / distance2);
                } else {
                    // if the object have texture
                    Texture &texture = scene.textures[intersectObject->texture_id];

                    DecalMode decalMode = texture.getDecalMode();

                    Vector3D textureColor = texture.getColor(texCoord, texture.getInterpolation());

                    switch (decalMode) {
                        case REPLACE_KD:
                            pixelColor =
                                    pixelColor + (textureColor / 256 * costheta_diff)
                                            .multiply(light.intensity / distance2);
                            break;
                        case BLEND_KD:
                            pixelColor =
                                    pixelColor +
                                    ((kDiffuse + textureColor / 256) * 0.5f * costheta_diff)
                                            .multiply(light.intensity / distance2);

                            break;
                        case REPLACE_ALL:
                            pixelColor = textureColor;
                            isReplaceAll = true;
                            break;
                    }
                }

                if (!isReplaceAll) {

                    // for specular
                    Vector3D halfVector = (wi + direction.inverse()).normalize();

                    float costheta_spec = std::max(float(0), normal.dotProduct(halfVector));

                    // specular shading
                    pixelColor = pixelColor
                                 + (kSpecular * powf(costheta_spec, scene.materials[intersectObject->material_id]
                            .phong_exponent)).multiply(light.intensity / distance2);
                }
            }
        }

        // mirror reflection
        if ((kMirror.x() + kMirror.y() + kMirror.z() > 0.001f)) {

            Vector3D reflection_direction = reflect(direction, normal);

            Vector3D ray_origin_with_epsilon = intersectPoint + reflection_direction * scene.shadow_ray_epsilon;

            pixelColor = pixelColor + kMirror.multiply(shade(scene, ray_origin_with_epsilon, reflection_direction,
                                                             backfaceCulling,
                                                             recursionDepth + 1));
        }

        // refraction and transparency
        if ((kTransparency.x() + kTransparency.y() + kTransparency.z() > 0.001f)) {

            Vector3D refractionColor;
            Vector3D reflectionColor;
            float kr;

            // compute fresnel
            fresnel(direction, normal, kRefraction, kr);
            bool is_outside = direction.dotProduct(normal) < 0;
            Vector3D bias = normal * kRefractionBias;

            // compute refraction if it is not a case of total internal reflection
            if (kr < 1) {
                Vector3D refractionDirection = refract(direction, normal, kRefraction).normalize();
                Vector3D refractionRayOrigin = is_outside ? intersectPoint - bias : intersectPoint + bias;
                refractionColor = shade(scene, refractionRayOrigin, refractionDirection, !is_outside, recursionDepth + 1);
            }

            Vector3D reflectionDirection = reflect(direction, normal);
            Vector3D reflectionRayOrigin = is_outside ? intersectPoint + bias : intersectPoint - bias;
            reflectionColor = shade(scene, reflectionRayOrigin, reflectionDirection, false, recursionDepth + 1);

            if (!is_outside) {
                // if the ray is inside, attenuation is applied
                Vector3D attenuation = kTransparency.power(tmin);

                refractionColor = refractionColor.multiply(attenuation);
                reflectionColor = reflectionColor.multiply(attenuation);

            }

            // mix the two using reflectance (kr) and transmittance (1 - kr)
            pixelColor = pixelColor + (reflectionColor * kr + refractionColor * (1 - kr));
        }

    } else {
        pixelColor.e[0] = scene.background_color.x; // r
        pixelColor.e[1] = scene.background_color.y; // g
        pixelColor.e[2] = scene.background_color.z; // b
    }
    return pixelColor;
}

void trace(parser::Scene *scene, parser::Camera *camera, int startHeight,
           int endHeight, int imageWidth, int imageHeight,
           unsigned char *image) {

    int i = startHeight * imageWidth * 3;

    float nearPlaneWidth = camera->near_plane.y - camera->near_plane.x;
    float nearPlaneHeight = camera->near_plane.w - camera->near_plane.z;

    for (int y = startHeight; y < endHeight; ++y) {
        for (int x = 0; x < imageWidth; ++x) {

            float pixelPositionX = nearPlaneWidth * (x + 0.5f) / imageWidth;   // su
            float pixelPositionY = nearPlaneHeight * (y + 0.5f) / imageHeight; // sv

            Vector3D pixelPosition = camera->planeStartPoint +
                                     (camera->right * pixelPositionX) -
                                     (camera->up * pixelPositionY);

            Vector3D direction = pixelPosition - camera->position;
            direction.normalize();

            Vector3D pixelColor = shade(*scene, camera->position, direction, true, 0);

            image[i++] = (unsigned char) std::min((int) std::round(pixelColor.x()), 255); // r
            image[i++] = (unsigned char) std::min((int) std::round(pixelColor.y()), 255); // g
            image[i++] = (unsigned char) std::min((int) std::round(pixelColor.z()), 255); // b
        }
    }
}
