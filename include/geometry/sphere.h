#pragma once

#include <memory>
#include <glm/glm.hpp>

#include "geometry/primitive.h"
#include "shading/material.h"
#include "accel/bbox.h"

class Sphere : public Primitive {
public:
    Sphere(const glm::dvec3& center,
           double radius,
           std::shared_ptr<Material> material);

    bool intersect(const Ray& ray, HitRecord& rec) const override;
    BBox boundingBox() const override;

    LightSample sampleLi(const glm::dvec3& shadingPoint,
                          double u1, double u2) const override;

    const std::shared_ptr<Material>& material() const override { return m_material; }

private:
    glm::dvec3 m_center;
    double m_radius;
    std::shared_ptr<Material> m_material;
};
