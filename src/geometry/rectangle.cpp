#include "geometry/rectangle.h"
#include "core/constants.h"
#include <algorithm>
#include <cmath>

Box::Face Box::makeFace(const glm::dvec3& origin, const glm::dvec3& e1,
                         const glm::dvec3& e2, const glm::dvec3& boxCenter) {
    glm::dvec3 n = glm::normalize(glm::cross(e1, e2));
    glm::dvec3 faceCenter = origin + 0.5 * e1 + 0.5 * e2;
    if (glm::dot(n, faceCenter - boxCenter) < 0.0)
        n = -n;

    Face f;
    f.origin = origin;
    f.e1 = e1;
    f.e2 = e2;
    f.normal = n;
    f.area = glm::length(glm::cross(e1, e2));
    return f;
}

bool Box::intersectFace(const Face& face, const Ray& ray, double tMin,
                         double tMaxLocal, double& outT, glm::dvec2& outUV) {
    double denom = glm::dot(face.normal, ray.direction);
    if (std::abs(denom) < constants::kEpsilon)
        return false;

    double t = glm::dot(face.origin - ray.origin, face.normal) / denom;
    if (t < tMin || t > tMaxLocal)
        return false;

    glm::dvec3 p = ray.at(t) - face.origin;

    double e1Len2 = glm::dot(face.e1, face.e1);
    double e2Len2 = glm::dot(face.e2, face.e2);
    double alpha = glm::dot(p, face.e1) / e1Len2;
    double beta  = glm::dot(p, face.e2) / e2Len2;

    if (alpha < 0.0 || alpha > 1.0 || beta < 0.0 || beta > 1.0)
        return false;

    outT = t;
    outUV = glm::dvec2(alpha, beta);
    return true;
}

Box::Box(const glm::dvec3 &corner, const glm::dvec3 &u, const glm::dvec3 &v,
         const glm::dvec3 &w, std::shared_ptr<Material> material)
    : m_corner(corner), m_u(u), m_v(v), m_w(w), m_material(std::move(material)) {
    buildFaces();
}

void Box::buildFaces() {
    glm::dvec3 center = m_corner + 0.5 * (m_u + m_v + m_w);

    // Boden (bei w=0) und Deckel (bei w=1)
    m_faces[0] = makeFace(m_corner, m_u, m_v, center);
    m_faces[1] = makeFace(m_corner + m_w, m_u, m_v, center);

    // Vorne (bei v=0) und hinten (bei v=1)
    m_faces[2] = makeFace(m_corner, m_u, m_w, center);
    m_faces[3] = makeFace(m_corner + m_v, m_u, m_w, center);

    // Links (bei u=0) und rechts (bei u=1)
    m_faces[4] = makeFace(m_corner, m_v, m_w, center);
    m_faces[5] = makeFace(m_corner + m_u, m_v, m_w, center);

    m_totalArea = 0.0;
    for (const Face& f : m_faces)
        m_totalArea += f.area;

    double cumulative = 0.0;
    for (size_t i = 0; i < m_faces.size(); ++i) {
        cumulative += m_faces[i].area;
        m_cumulativeAreaFrac[i] = cumulative / m_totalArea;
    }
}

bool Box::intersect(const Ray &ray, HitRecord &rec) const {
    bool hitAnything = false;
    double closestT = ray.tMax;
    glm::dvec2 bestUV;
    const Face* bestFace = nullptr;

    for (const Face& face : m_faces) {
        double t;
        glm::dvec2 uv;
        if (intersectFace(face, ray, ray.tMin, closestT, t, uv)) {
            hitAnything = true;
            closestT = t;
            bestUV = uv;
            bestFace = &face;
        }
    }

    if (!hitAnything)
        return false;

    rec.t = closestT;
    rec.position = ray.at(closestT);
    rec.setFaceNormal(ray.direction, bestFace->normal);
    rec.material = m_material;
    rec.uv = bestUV;

    return true;
}

BBox Box::boundingBox() const {
    std::array<glm::dvec3, 8> corners = {
        m_corner,
        m_corner + m_u,
        m_corner + m_v,
        m_corner + m_w,
        m_corner + m_u + m_v,
        m_corner + m_u + m_w,
        m_corner + m_v + m_w,
        m_corner + m_u + m_v + m_w,
    };

    glm::dvec3 pmin = corners[0];
    glm::dvec3 pmax = corners[0];
    for (const glm::dvec3& c : corners) {
        pmin = glm::min(pmin, c);
        pmax = glm::max(pmax, c);
    }

    // Ein echter Quader hat in jeder Achse Ausdehnung > 0, daher ist hier
    // (anders als beim flachen Rectangle) kein künstliches Padding nötig.
    return BBox(pmin, pmax);
}

LightSample Box::sampleLi(const glm::dvec3 &shadingPoint,
                            double u1, double u2) const {
    // Fläche proportional zu ihrem Flächenanteil auswählen und u1 auf den
    // lokalen Bereich [0,1) dieser Fläche remappen (stratifiziertes
    // Sampling über die Gesamtoberfläche des Quaders).
    size_t faceIdx = 0;
    for (; faceIdx < m_faces.size(); ++faceIdx) {
        if (u1 < m_cumulativeAreaFrac[faceIdx])
            break;
    }
    faceIdx = std::min(faceIdx, m_faces.size() - 1);

    double lo = (faceIdx == 0) ? 0.0 : m_cumulativeAreaFrac[faceIdx - 1];
    double hi = m_cumulativeAreaFrac[faceIdx];
    double localU1 = (hi > lo) ? (u1 - lo) / (hi - lo) : 0.0;

    const Face& face = m_faces[faceIdx];
    glm::dvec3 pos = face.origin + localU1 * face.e1 + u2 * face.e2;

    glm::dvec3 wi = pos - shadingPoint;
    double dist2 = glm::dot(wi, wi);
    double dist = std::sqrt(dist2);
    glm::dvec3 wiNorm = wi / dist;

    double cosAtLight = std::max(1e-6, std::abs(glm::dot(face.normal, wiNorm)));

    LightSample s;
    s.position = pos;
    s.normal = face.normal;
    // Da die Fläche proportional zu ihrer Größe gewählt wurde, entspricht
    // das insgesamt einem gleichverteilten Sampling über die gesamte
    // Quaderoberfläche -> Gesamtfläche statt Einzelflächenfläche im pdf.
    s.pdf = dist2 / (cosAtLight * m_totalArea);
    return s;
}
