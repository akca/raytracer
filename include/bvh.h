#ifndef RAYTRACER_BVH_H
#define RAYTRACER_BVH_H

#import "object.h"

class BVH : public Object {
public:
    BVH();

    BVH(Object **surfaces, int num_surfaces);

    BVH(Object *prim1, Object *prim2);

    BVH(Object *prim1, Object *prim2, const BBox &_bbox);

    bool intersects(const Ray &ray, float &tmin, HitRecord &hit_record, bool backfaceCulling) override;

//    bool shadowHit(const Ray &r, float tmin, float tmax, float time) const;

    BBox boundingBox(float timeO, float time1) override;

    Object *buildBranch(Object **surfaces, int num_surfaces, int axis = 0);

    BBox bbox;
    Object *left;
    Object *right;
};

inline BVH::BVH(Object *prim1, Object *prim2, const BBox &_bbox) {
    bbox = _bbox;
    left = prim1;
    right = prim2;
}

inline BVH::BVH(Object *prim1, Object *prim2) {
    left = prim1;
    right = prim2;
    bbox = surround(prim1->boundingBox(0.0f, 0.0f),
                    prim2->boundingBox(0.0f, 0.0f));
}


#endif //RAYTRACER_BVH_H
