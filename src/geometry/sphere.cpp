// Sphere intersection implementation.

#include "geometry/sphere.h"
#include "glm/detail/func_geometric.hpp"

#include <cmath>
#include <glm/glm.hpp>

Sphere::Sphere(const glm::dvec3 &center, double radius,
               std::shared_ptr<Material> material)
    : m_center(center), m_radius(radius), m_material(std::move(material)) {}

bool Sphere::intersect(const Ray &ray, HitRecord &rec) const {
  glm::dvec3 oc = ray.origin - m_center;

  // Solve dot(ray.direction, ray.direction) * t² + 2 * dot(ray.direction,
  // ray.origin) * t + dot(ray.origin, ray.origin) - r² = 0
  auto a = glm::dot(ray.direction, ray.direction);
  auto b = glm::dot(ray.direction, oc);
  auto c = glm::dot(oc, oc) - pow(m_radius, 2);
  auto discriminant = b * b - a * c;

  bool hit = discriminant >= 0;

  if (hit) {
    auto rootDiscrim = std::sqrt(discriminant);
    auto q = -(b + std::copysign(rootDiscrim, b));
    auto t0 = q / a;
    auto t1 = c / q;

    if (t0 > t1) {
      std::swap(t0, t1);
    }

    rec.t = t0;
    rec.material = m_material;
    rec.position = ray.at(t0);
    rec.frontFace = true;
    rec.setFaceNormal(ray.direction, (rec.position - m_center) / m_radius);
  }

  return hit;
}

GPUSphere Sphere::toGPU(int materialIndex) const {
  GPUSphere gpu{};
  gpu.center = glm::vec4(m_center.x, m_center.y, m_center.z, m_radius);
  gpu.materialIndex = materialIndex;
  return gpu;
}

BBox Sphere::boundingBox() const
{
    glm::dvec3 r(m_radius);

    return BBox(
        m_center - r,
        m_center + r
    );
}
