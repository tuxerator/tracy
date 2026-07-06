#pragma once
#include <memory>
#include "core/ray.h"
#include "scene/hit_record.h"

class BBox;
class Material;

struct LightSample {
    glm::dvec3 position;
    glm::dvec3 normal;
    double pdf; // bezogen auf Raumwinkel vom shading point aus
};

class Primitive {
public:
    virtual ~Primitive() noexcept = default;
    virtual bool intersect(const Ray& ray, HitRecord& rec) const = 0;
    virtual const std::shared_ptr<Material>& material() const = 0;
    virtual BBox boundingBox() const = 0;

    // Sampled einen Punkt auf der Oberfläche als Lichtquelle, gesehen von shadingPoint.
    // Standardimplementierung: keine Fläche samplebar -> pdf 0.
    virtual LightSample sampleLi(const glm::dvec3& shadingPoint,
                                  double u1, double u2) const {
        (void)shadingPoint; (void)u1; (void)u2;
        return LightSample{glm::dvec3(0.0), glm::dvec3(0.0, 1.0, 0.0), 0.0};
    }
};
