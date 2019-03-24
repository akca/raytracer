#ifndef OBJECT_H__
#define OBJECT_H__

#include "utility.h"
#include "vector3d.h"
#include "bbox.h"
#include "texture.h"
#include <string>
#include <vector>

struct HitRecord {
    float t;
    Vector3D normal;
    Vector3D hit_p; // the point of intersection
    Texture *hit_tex; // the nearest intersected object's texture
};

class Object {
public:
    virtual ~Object() {}

    virtual bool intersects(const Ray &, float &,
                            Vector3D &intersectPoint, Vector3D &,
                            bool backfaceCulling, Vec2f &texCoordData) = 0;

    virtual BBox boundingBox(float x, float y) = 0;

    int material_id;
    int texture_id;
};

#endif
