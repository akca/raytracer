#include <bvh.h>

bool BVH::bounding_box(float t0, float t1, BBox &box) {
    box = bbox;
    return true;
}

bool BVH::intersects(const Ray &ray, float tmin, float tmax, HitRecord &hit_record, bool backfaceCulling) {

    if (!bbox.rayIntersect(ray, tmin, tmax)) {
        return false;
    }

    HitRecord left_rec, right_rec;

    bool hit_left = left->intersects(ray, tmin, tmax, left_rec, backfaceCulling);
    bool hit_right = right->intersects(ray, tmin, tmax, right_rec, backfaceCulling);

    if (hit_left && hit_right) {
        if (left_rec.t < right_rec.t)
            hit_record = left_rec;
        else
            hit_record = right_rec;
        return true;
    } else if (hit_left) {
        hit_record = left_rec;
        return true;
    } else if (hit_right) {
        hit_record = right_rec;
        return true;
    } else
        return false;
}

int box_x_compare(const void *a, const void *b) {
    BBox box_left, box_right;
    Object *ah = *(Object **) a;
    Object *bh = *(Object **) b;
    if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
        std::cerr << "no bounding box in bvh_node constructor\n";
    if (box_left.min().x() - box_right.min().x() < 0.0)
        return -1;
    else
        return 1;
}

int box_y_compare(const void *a, const void *b) {
    BBox box_left, box_right;
    Object *ah = *(Object **) a;
    Object *bh = *(Object **) b;
    if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
        std::cerr << "no bounding box in bvh_node constructor\n";
    if (box_left.min().y() - box_right.min().y() < 0.0)
        return -1;
    else
        return 1;
}

int box_z_compare(const void *a, const void *b) {
    BBox box_left, box_right;
    Object *ah = *(Object **) a;
    Object *bh = *(Object **) b;
    if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
        std::cerr << "no bounding box in bvh_node constructor\n";
    if (box_left.min().z() - box_right.min().z() < 0.0)
        return -1;
    else
        return 1;
}

BVH::BVH(Object **shapes, size_t num_shapes) {

    int axis = int(3 * drand48());
    if (axis == 0)
        qsort(shapes, num_shapes, sizeof(Object *), box_x_compare);
    else if (axis == 1)
        qsort(shapes, num_shapes, sizeof(Object *), box_y_compare);
    else
        qsort(shapes, num_shapes, sizeof(Object *), box_z_compare);

    if (num_shapes == 1) {
        left = right = shapes[0];
    } else if (num_shapes == 2) {
        left = shapes[0];
        right = shapes[1];
    } else {
        left = new BVH(shapes, num_shapes / 2);
        right = new BVH(shapes + num_shapes / 2, num_shapes - num_shapes / 2);
    }

    BBox box_left, box_right;

    if (!left->bounding_box(0, 0, box_left) || !right->bounding_box(0, 0, box_right))
        std::cerr << "no bounding box in bvh_node constructor\n";
    bbox = surround(box_left, box_right);
}