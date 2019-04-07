#ifndef OBJECT_H__
#define OBJECT_H__

#include "utility.h"
#include "vector3d.h"
#include "bbox.h"
#include "texture.h"
#include <string>
#include <vector>

struct HitRecord
{
    float t = std::numeric_limits<float>::max();
    Vec2f texture_coords;
    int material_id = -1;
    int texture_id = -1;
    Vector3D intersection_point;
    Vector3D normal;
};

class Object {
public:
    virtual ~Object() {}

    virtual bool intersects(const Ray &ray, float tmin, float tmax, HitRecord& hit_record, bool backfaceCulling) = 0;

    virtual bool bounding_box(float t0, float t1, BBox& box) = 0;

    int material_id = -1;
    int texture_id = -1;
};

#endif
