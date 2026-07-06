#include "geometry/rectangle.h"
#include "core/constants.h"
#include <cmath>

Rectangle::Rectangle(const glm::dvec3 &corner, const glm::dvec3 &u,
                      const glm::dvec3 &v, std::shared_ptr<Material> material)
    : m_corner(corner), m_u(u), m_v(v), m_material(std::move(material)) {
    m_normal = glm::normalize(glm::cross(u, v));
    m_area = glm::length(glm::cross(u, v));
}

bool Rectangle::intersect(const Ray &ray, HitRecord &rec) const {
    double denom = glm::dot(m_normal, ray.direction);
    if (std::abs(denom) < constants::kEpsilon)
        return false;

    double t = glm::dot(m_corner - ray.origin, m_normal) / denom;
    if (t < ray.tMin || t > ray.tMax)
        return false;

    glm::dvec3 p = ray.at(t) - m_corner;

    // Projektion auf u/v-Achsen, um zu prüfen ob innerhalb des Rechtecks
    double uLen2 = glm::dot(m_u, m_u);
    double vLen2 = glm::dot(m_v, m_v);
    double alpha = glm::dot(p, m_u) / uLen2;
    double beta  = glm::dot(p, m_v) / vLen2;

    if (alpha < 0.0 || alpha > 1.0 || beta < 0.0 || beta > 1.0)
        return false;

    rec.t = t;
    rec.position = ray.at(t);
    rec.setFaceNormal(ray.direction, m_normal);
    rec.material = m_material;
    rec.uv = glm::dvec2(alpha, beta);

    return true;
}

BBox Rectangle::boundingBox() const {
    glm::dvec3 p0 = m_corner;
    glm::dvec3 p1 = m_corner + m_u;
    glm::dvec3 p2 = m_corner + m_v;
    glm::dvec3 p3 = m_corner + m_u + m_v;

    glm::dvec3 pmin = glm::min(glm::min(p0, p1), glm::min(p2, p3));
    glm::dvec3 pmax = glm::max(glm::max(p0, p1), glm::max(p2, p3));

    // Kleine Ausdehnung entlang der Normalen, falls Rechteck achsenparallel
    // liegt (sonst hätte die BBox in einer Achse Dicke 0, was manche BVH
    // Implementierungen mögen, andere nicht)
    glm::dvec3 pad = m_normal * 1e-4;
    return BBox(pmin - pad, pmax + pad);
}

LightSample Rectangle::sampleLi(const glm::dvec3 &shadingPoint,
                                  double u1, double u2) const {
    glm::dvec3 pos = m_corner + u1 * m_u + u2 * m_v;

    glm::dvec3 wi = pos - shadingPoint;
    double dist2 = glm::dot(wi, wi);
    double dist = std::sqrt(dist2);
    glm::dvec3 wiNorm = wi / dist;

    double cosAtLight = std::max(1e-6, std::abs(glm::dot(m_normal, wiNorm)));

    LightSample s;
    s.position = pos;
    s.normal = m_normal;
    s.pdf = dist2 / (cosAtLight * m_area);
    return s;
}