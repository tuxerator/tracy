#pragma once

#include <memory>
#include <glm/glm.hpp>

#include "geometry/primitive.h"
#include "shading/material.h"

struct GPUSphere {
    glm::vec4 center;
    int materialIndex;
    int _pad[3];
};
static_assert(sizeof(GPUSphere) == 32);

class Sphere : public Primitive {
public:
    Sphere(const glm::dvec3& center,
           double radius,
           std::shared_ptr<Material> material);

    bool intersect(const Ray& ray, HitRecord& rec) const override;

    const std::shared_ptr<Material>& material() const override { return m_material; }

    GPUSphere toGPU(int materialIndex) const;

private:
    glm::dvec3 m_center;
    double m_radius;
    std::shared_ptr<Material> m_material;
};
