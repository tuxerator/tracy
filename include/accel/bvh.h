// Stub BVH interface kept in the starter for Direction 1.

#pragma once

#include "accel/accel_structure.h"
#include "geometry/primitive.h"
#include "accel/bbox.h"

#include <vector>
#include <memory>

class BVH : public AccelStructure {
public:
    bool intersect(const Ray& ray, HitRecord& rec) const override;
};

class BVHNode : public Primitive
{
public:
    ~BVHNode() noexcept override = default;
    BVHNode(
        std::vector<std::shared_ptr<Primitive>>& objects,
        size_t start,
        size_t end);

    bool intersect(
        const Ray& ray,
        HitRecord& rec) const override;

    BBox boundingBox() const override;

private:
    std::shared_ptr<Primitive> left;
    std::shared_ptr<Primitive> right;

    BBox box;
};
