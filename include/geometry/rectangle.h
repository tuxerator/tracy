#pragma once

#include <memory>
#include <glm/glm.hpp>

#include "geometry/primitive.h"
#include "shading/material.h"
#include "accel/bbox.h"

// Endliches, achsenbeliebig orientiertes Rechteck, aufgespannt durch
// zwei Kantenvektoren u, v ausgehend von einer Ecke (corner).
class Rectangle : public Primitive {
public:
    Rectangle(const glm::dvec3& corner,
               const glm::dvec3& u,
               const glm::dvec3& v,
               std::shared_ptr<Material> material);

    bool intersect(const Ray& ray, HitRecord& rec) const override;
    BBox boundingBox() const override;
    LightSample sampleLi(const glm::dvec3& shadingPoint,
                          double u1, double u2) const override;

    const std::shared_ptr<Material>& material() const override { return m_material; }

private:
    glm::dvec3 m_corner;
    glm::dvec3 m_u, m_v;      // Kantenvektoren
    glm::dvec3 m_normal;
    double m_area;
    std::shared_ptr<Material> m_material;
};