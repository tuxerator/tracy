// Sphere intersection implementation.

#include "geometry/sphere.h"

#include <glm/glm.hpp>
#include <cmath>

Sphere::Sphere(const glm::dvec3& center,
               double radius,
               std::shared_ptr<Material> material)
    : m_center(center), m_radius(radius), m_material(std::move(material)) {}

bool Sphere::intersect(const Ray& ray, HitRecord& rec) const {
    glm::dvec3 oc = ray.origin - m_center;

    double a = glm::dot(ray.direction, ray.direction);
    double b = 2.0 * glm::dot(oc, ray.direction);
    double c = glm::dot(oc, oc) - m_radius * m_radius;

    double discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return false;
    }

    double sqrtD = std::sqrt(discriminant);
    double t = (-b - sqrtD) / (2.0 * a);

    if (t < 0) {
        t = (-b + sqrtD) / (2.0 * a);
        if (t < 0) {
            return false;
        }
    }

    // HitRecord
    rec.t = t;
    rec.position = ray.origin + t * ray.direction;
    glm::dvec3 outwardNormal = (rec.position - m_center) / m_radius;
    rec.setFaceNormal(ray.direction, glm::normalize(outwardNormal));
    rec.material = m_material;

    return true;
}

BBox Sphere::boundingBox() const
{
    glm::dvec3 r(m_radius);

    return BBox(
        m_center - r,
        m_center + r
    );
}
