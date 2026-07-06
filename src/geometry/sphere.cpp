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

    if (t0 < 1e-12) {
      return false;
    }

    rec.t = t0;
    rec.material = m_material;
    rec.position = ray.at(t0);
    rec.frontFace = true;
    rec.setFaceNormal(ray.direction,
                      glm::normalize((rec.position - m_center) / m_radius));
  }

  return hit;
}

BBox Sphere::boundingBox() const {
  glm::dvec3 r(m_radius);

  return BBox(m_center - r, m_center + r);
}

LightSample Sphere::sampleLi(const glm::dvec3 &shadingPoint,
                              double u1, double u2) const {
    glm::dvec3 toCenter = m_center - shadingPoint;
    double distToCenter2 = glm::dot(toCenter, toCenter);
    double r2 = m_radius * m_radius;

    // Fallback: shadingPoint liegt innerhalb der Kugel -> uniform über volle Kugel
    if (distToCenter2 <= r2) {
        double z = 1.0 - 2.0 * u1;
        double r = std::sqrt(std::max(0.0, 1.0 - z * z));
        double phi = 2.0 * M_PI * u2;
        glm::dvec3 n(r * std::cos(phi), r * std::sin(phi), z);
        glm::dvec3 pos = m_center + m_radius * n;

        glm::dvec3 wi = pos - shadingPoint;
        double dist2 = glm::dot(wi, wi);
        double cosAtLight = std::max(1e-6, glm::dot(n, -glm::normalize(wi)));

        LightSample s;
        s.position = pos;
        s.normal = n;
        s.pdf = dist2 / (cosAtLight * 4.0 * M_PI * r2);
        return s;
    }

    // Kegel-Sampling: nur die von shadingPoint aus sichtbare Kappe
    double distToCenter = std::sqrt(distToCenter2);
    double sinThetaMax2 = r2 / distToCenter2;
    double cosThetaMax = std::sqrt(std::max(0.0, 1.0 - sinThetaMax2));

    double cosTheta = 1.0 - u1 * (1.0 - cosThetaMax);
    double sinTheta = std::sqrt(std::max(0.0, 1.0 - cosTheta * cosTheta));
    double phi = 2.0 * M_PI * u2;

    glm::dvec3 wc = toCenter / distToCenter;
    glm::dvec3 a = std::abs(wc.x) > 0.9 ? glm::dvec3(0, 1, 0) : glm::dvec3(1, 0, 0);
    glm::dvec3 tangent = glm::normalize(glm::cross(a, wc));
    glm::dvec3 bitangent = glm::cross(wc, tangent);

    glm::dvec3 sampleDir =
        tangent * (sinTheta * std::cos(phi)) +
        bitangent * (sinTheta * std::sin(phi)) +
        wc * cosTheta;

    double ds = distToCenter * cosTheta -
        std::sqrt(std::max(0.0, r2 - distToCenter2 * sinTheta * sinTheta));
    glm::dvec3 pos = shadingPoint + ds * sampleDir;
    glm::dvec3 n = glm::normalize(pos - m_center);

    LightSample s;
    s.position = pos;
    s.normal = n;
    s.pdf = 1.0 / (2.0 * M_PI * (1.0 - cosThetaMax));
    return s;
}