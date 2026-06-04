// Starter stub for a BVH acceleration structure.

#include "accel/bvh.h"

#include <algorithm>

bool BVH::intersect(const Ray&, HitRecord&) const {
    return false;
}

static BBox surrounding(const BBox& a, const BBox& b) {
    glm::dvec3 small(
        std::min(a.min().x, b.min().x),
        std::min(a.min().y, b.min().y),
        std::min(a.min().z, b.min().z)
    );
    glm::dvec3 big(
        std::max(a.max().x, b.max().x),
        std::max(a.max().y, b.max().y),
        std::max(a.max().z, b.max().z)
    );
    return BBox(small, big);
}

BVHNode::BVHNode(
    std::vector<std::shared_ptr<Primitive>>& objects,
    size_t start,
    size_t end)
{
    size_t count = end - start;

    if (count == 1)
    {
        left = right = objects[start];
    }
    else if (count == 2)
    {
        left  = objects[start];
        right = objects[start + 1];
    }
    else
    {
        int axis = rand() % 3;
        std::sort(objects.begin() + start, objects.begin() + end,
            [axis](const auto& a, const auto& b) {
                return a->boundingBox().centroid()[axis]
                    < b->boundingBox().centroid()[axis];
            });
        size_t mid = start + count / 2;

        left  = std::make_shared<BVHNode>(objects, start, mid);
        right = std::make_shared<BVHNode>(objects, mid,   end);
    }

    // Box aus den Kindknoten berechnen
    box = surrounding(left->boundingBox(), right->boundingBox());
}

BBox BVHNode::boundingBox() const
{
    return box;
}

bool BVHNode::intersect(
    const Ray& ray,
    HitRecord& rec) const
{
    if (!box.intersect(ray))
    {
        return false;
    }

    HitRecord leftRec;
    HitRecord rightRec;

    bool hitLeft =
        left->intersect(ray, leftRec);

    bool hitRight =
        right->intersect(ray, rightRec);

    if (hitLeft && hitRight)
    {
        if (leftRec.t < rightRec.t)
        {
            rec = leftRec;
        }
        else
        {
            rec = rightRec;
        }

        return true;
    }

    if (hitLeft)
    {
        rec = leftRec;
        return true;
    }

    if (hitRight)
    {
        rec = rightRec;
        return true;
    }

    return false;
}
