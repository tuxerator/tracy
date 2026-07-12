#pragma once

#include <array>
#include <memory>
#include <glm/glm.hpp>

#include "geometry/primitive.h"
#include "shading/material.h"
#include "accel/bbox.h"

// Dreidimensionaler Quader (Box), aufgespannt durch drei Kantenvektoren
// u, v, w ausgehend von einer Ecke (corner). Intern besteht der Quader
// aus 6 rechteckigen (parallelogrammförmigen) Flächen.
//
// Hinweis: u, v, w müssen nicht zwingend orthogonal sein (dann ergibt
// sich ein allgemeines Parallelepiped), für einen "echten" Quader sollten
// sie paarweise senkrecht stehen.
class Box : public Primitive {
public:
    Box(const glm::dvec3& corner,
        const glm::dvec3& u,
        const glm::dvec3& v,
        const glm::dvec3& w,
        std::shared_ptr<Material> material);

    bool intersect(const Ray& ray, HitRecord& rec) const override;
    BBox boundingBox() const override;
    LightSample sampleLi(const glm::dvec3& shadingPoint,
                          double u1, double u2) const override;

    const std::shared_ptr<Material>& material() const override { return m_material; }

private:
    // Eine einzelne Rechteck-Fläche des Quaders.
    struct Face {
        glm::dvec3 origin;   // Eckpunkt der Fläche
        glm::dvec3 e1, e2;   // Kantenvektoren der Fläche
        glm::dvec3 normal;   // nach außen zeigende Normale
        double area = 0.0;
    };

    glm::dvec3 m_corner;
    glm::dvec3 m_u, m_v, m_w;      // Kantenvektoren des Quaders
    std::shared_ptr<Material> m_material;

    std::array<Face, 6> m_faces;
    double m_totalArea = 0.0;
    // kumulierte, auf [0,1] normierte Flächenanteile für das Sampling
    std::array<double, 6> m_cumulativeAreaFrac{};

    void buildFaces();

    // Baut eine einzelne Face und richtet die Normale anhand des
    // Box-Zentrums nach außen aus.
    static Face makeFace(const glm::dvec3& origin, const glm::dvec3& e1,
                          const glm::dvec3& e2, const glm::dvec3& boxCenter);

    // Prüft eine einzelne Rechteck-Fläche auf Schnitt mit dem Ray, analog
    // zur ursprünglichen Rectangle::intersect-Logik. tMaxLocal begrenzt
    // die Suche nach oben (für Closest-Hit unter mehreren Flächen).
    static bool intersectFace(const Face& face, const Ray& ray, double tMin,
                               double tMaxLocal, double& outT, glm::dvec2& outUV);
};
