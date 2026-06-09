// Infinite plane intersection implementation.

#include "geometry/plane.h"

#include <cmath>
#include <glm/glm.hpp>

#include "core/constants.h"

Plane::Plane(const glm::dvec3 &point, const glm::dvec3 &normal,
             std::shared_ptr<Material> material)
    : m_point(point), m_normal(glm::normalize(normal)),
      m_material(std::move(material)) {}

bool Plane::intersect(const Ray &ray, HitRecord &rec) const {
  double denom = glm::dot(m_normal, ray.direction);

  // Ray paralell to plane
  if (std::abs(denom) < constants::kEpsilon) {
    return false;
  }

  double t = glm::dot(m_point - ray.origin, m_normal) / denom;
  if (t < ray.tMin || t > ray.tMax) {
    return false;
  }

  rec.t = t;
  rec.position = ray.at(t);
  rec.setFaceNormal(ray.direction, m_normal);
  rec.material = m_material;
  rec.uv = glm::dvec2(rec.position.x, rec.position.z);

  return true;
}

GPUPlane Plane::toGPU(int materialIndex) const {
  GPUPlane gpu{};
  gpu.point = glm::vec4(m_point.x, m_point.y, m_point.z, 0.0f);
  gpu.normal = glm::vec4(m_normal.x, m_normal.y, m_normal.z, 0.0f);
  gpu.materialIndex = materialIndex;
  return gpu;
}

