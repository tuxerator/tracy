#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <vector>

#include "geometry/primitive.h"
#include "shading/material.h"
#include "accel/bbox.h"

struct GPUTriangle {
    glm::vec4 a;
    glm::vec4 b;
    glm::vec4 c;
    int materialIndex;
    int _pad[3];
};
static_assert(sizeof(GPUTriangle) == 64);

class Triangle : public Primitive {
public:
    Triangle(
        const glm::dvec3& v0,
        const glm::dvec3& v1,
        const glm::dvec3& v2,

        const glm::dvec2& uv0,
        const glm::dvec2& uv1,
        const glm::dvec2& uv2,

        std::shared_ptr<Material> material
    );

    Triangle(
        const glm::dvec3& v0,
        const glm::dvec3& v1,
        const glm::dvec3& v2,

        std::shared_ptr<Material> material
    );

    bool intersect(const Ray& ray, HitRecord& rec) const override;

    const std::shared_ptr<Material>& material() const override { return m_material; }
    BBox boundingBox() const override;

    GPUTriangle toGPU(int materialIndex) const;

private:
    glm::dvec3 m_a;
    glm::dvec3 m_b;
    glm::dvec3 m_c;
    const glm::dvec2 m_uv0{0.0, 0.0};
    const glm::dvec2 m_uv1{1.0, 0.0};
    const glm::dvec2 m_uv2{0.5, 1.0};
    std::shared_ptr<Material> m_material;
};
