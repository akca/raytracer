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
    float t;
    Vec2f texture_coords;
    int material_id = -1;
    int texture_id = -1;
    Vector3D intersection_point;
    Vector3D normal;
};

class Object {
public:
    virtual ~Object() {}

    virtual bool intersects(const Ray &ray, float &t, HitRecord& hit_record, bool backfaceCulling) = 0;

    virtual BBox boundingBox(float x, float y) = 0;

    int material_id;
    int texture_id = -1;
};

#endif
