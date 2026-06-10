#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "geometry/triangle.h"
#include "geometry/primitive.h"

class Mesh : public Primitive {
public:
    void addTriangle(const Triangle& tri);

    bool intersect(const Ray& ray, HitRecord& rec) const override;

private:
    std::vector<Triangle> m_triangles;
};