#include "../include/parser.h"
#include <thread>
#include <algorithm>
#include <random>

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

Vector3D
shade(parser::Scene &scene, Ray &ray, bool backfaceCulling, int recursionDepth) {

    if (recursionDepth > scene.max_recursion_depth)
        return Vector3D(scene.background_color);

    float tmin = 0.000001;
    float tmax = std::numeric_limits<float>::max();
    Vector3D pixelColor;
    HitRecord hit_record;

    // if intersection with an object found
    if (scene.root_bvh->intersects(ray, tmin, tmax, hit_record, backfaceCulling)) {

        hit_record.intersection_point = ray.origin + ray.direction * hit_record.t;

        Vector3D &kAmbient = scene.materials[hit_record.material_id].ambient;

        // ambient light
        pixelColor = kAmbient.multiply(scene.ambient_light);

        for (auto &light : scene.point_lights) {

            Vector3D wi = light.position - hit_record.intersection_point;
            float lightDistance = hit_record.intersection_point.distance(light.position);
            wi = wi / lightDistance; // normalize

            Vector3D shadowRayOrigin = hit_record.intersection_point + wi * scene.shadow_ray_epsilon;

            // search objects between light and intersection point
            float stmin = lightDistance;
            HitRecord hit_record_shadow;

            bool underShadow = false;

            // shadow ray
            Ray shadow_ray(shadowRayOrigin, wi, ray.time);

            if (scene.root_bvh->intersects(shadow_ray, 0, stmin, hit_record_shadow, false)) {
                underShadow = true;
            }

            if (!underShadow) {
                bool isReplaceAll = false;

                float costheta_diff = std::max(float(0), wi.dotProduct(hit_record.normal));
                float distance2 = powf(lightDistance, 2);
                Vector3D &kDiffuse = scene.materials[hit_record.material_id].diffuse;

                // diffuse shading
                if (hit_record.texture_id == -1) {
                    pixelColor =
                            pixelColor +
                            (kDiffuse * costheta_diff).multiply(light.intensity / distance2);
                } else {
                    // if the object have texture
                    Texture &texture = scene.textures[hit_record.texture_id];

                    DecalMode decalMode = texture.getDecalMode();

                    Vector3D textureColor = texture.getColor(hit_record.texture_coords, texture.getInterpolation());

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
                    Vector3D halfVector = (wi + ray.direction.inverse()).normalize();

                    float costheta_spec = std::max(float(0), hit_record.normal.dotProduct(halfVector));

                    // specular shading
                    pixelColor = pixelColor
                                 + (scene.materials[hit_record.material_id].specular
                                    * powf(costheta_spec,
                                           scene.materials[hit_record.material_id].phong_exponent)).multiply(
                            light.intensity / distance2);
                }
            }
        }

        // mirror reflection
        Vector3D &kMirror = scene.materials[hit_record.material_id].mirror;

        if ((kMirror.x() + kMirror.y() + kMirror.z() > 0.001f)) {

            const Vector3D reflection_direction = reflect(ray.direction, hit_record.normal);

            const Vector3D ray_origin_with_epsilon =
                    hit_record.intersection_point + reflection_direction * scene.shadow_ray_epsilon;

            Ray reflection_ray = Ray(ray_origin_with_epsilon, reflection_direction, ray.time);

            pixelColor = pixelColor + kMirror.multiply(shade(scene, reflection_ray,
                                                             backfaceCulling,
                                                             recursionDepth + 1));
        }

        // refraction and transparency
        Vector3D &kTransparency = scene.materials[hit_record.material_id].transparency;

        if ((kTransparency.x() + kTransparency.y() + kTransparency.z() > 0.001f)) {

            Vector3D refractionColor;
            Vector3D reflectionColor;
            float kr;
            const float &kRefraction = scene.materials[hit_record.material_id].refraction_index;

            // compute fresnel
            fresnel(ray.direction, hit_record.normal, kRefraction, kr);
            bool is_outside = ray.direction.dotProduct(hit_record.normal) < 0;
            Vector3D bias = hit_record.normal * kRefractionBias;

            // compute refraction if it is not a case of total internal reflection
            if (kr < 1) {
                Vector3D refractionDirection = refract(ray.direction, hit_record.normal, kRefraction).normalize();
                Vector3D refractionRayOrigin = is_outside ? hit_record.intersection_point - bias :
                                               hit_record.intersection_point + bias;

                Ray refraction_ray = Ray(refractionRayOrigin, refractionDirection, ray.time);

                refractionColor = shade(scene, refraction_ray, !is_outside, recursionDepth + 1);
            }

            Vector3D reflectionDirection = reflect(ray.direction, hit_record.normal);
            Vector3D reflectionRayOrigin = is_outside ? hit_record.intersection_point + bias :
                                           hit_record.intersection_point - bias;

            Ray reflection_ray = Ray(reflectionRayOrigin, reflectionDirection, ray.time);

            reflectionColor = shade(scene, reflection_ray, false, recursionDepth + 1);

            if (!is_outside) {
                // if the ray is inside, attenuation is applied
                Vector3D attenuation = kTransparency.power(hit_record.t);

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

void trace(parser::Scene *scene, Camera *camera, int startHeight,
           int endHeight, int imageWidth, int imageHeight,
           unsigned char *image) {

    int i = startHeight * imageWidth * 3;

    // set-up random device
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    int sqrtSamples = sqrt(camera->num_samples);
    int sqrtSamplesSquared = sqrtSamples * sqrtSamples;

    // iterate over pixels
    for (int y = startHeight; y < endHeight; ++y) {
        for (int x = 0; x < imageWidth; ++x) {

            Vector3D pixelColor;

            // jittered sampling for antialiasing
            for (int p = 0; p < sqrtSamples; p++) {
                for (int q = 0; q < sqrtSamples; q++) {

                    float pixelPositionX, pixelPositionY;

                    if (camera->num_samples > 1) {
                        pixelPositionX = (x + (p + dist(mt)) / sqrtSamples)
                                         / imageWidth;  // su

                        pixelPositionY = (y + (q + dist(mt)) / sqrtSamples)
                                         / imageHeight; // sv
                    } else {
                        pixelPositionX = (x + 0.5f) / imageWidth;   // su
                        pixelPositionY = (y + 0.5f) / imageHeight; // sv
                    }

                    Ray ray;

                    camera->createRay(Vec2f(pixelPositionX, pixelPositionY), ray);

                    pixelColor = pixelColor + shade(*scene, ray, true, 0);
                }
            }

            pixelColor = pixelColor / sqrtSamplesSquared;

            image[i++] = (unsigned char) std::min((int) std::round(pixelColor.x()), 255); // r
            image[i++] = (unsigned char) std::min((int) std::round(pixelColor.y()), 255); // g
            image[i++] = (unsigned char) std::min((int) std::round(pixelColor.z()), 255); // b
        }
    }
}
