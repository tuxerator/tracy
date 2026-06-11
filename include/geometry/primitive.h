#pragma once

#include <memory>

#include "core/ray.h"
#include "scene/hit_record.h"

class BBox;
class Material;

class Primitive {
public:
    virtual ~Primitive() noexcept = default;

    virtual bool intersect(const Ray& ray, HitRecord& rec) const = 0;

    virtual const std::shared_ptr<Material>& material() const = 0;
    virtual BBox boundingBox() const = 0;
};
