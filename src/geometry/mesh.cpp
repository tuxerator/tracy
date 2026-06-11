#include "geometry/mesh.h"

#include <glm/glm.hpp>

void Mesh::addTriangle(const Triangle& tri)
{
    m_triangles.push_back(tri);
}

bool Mesh::intersect(const Ray& ray, HitRecord& rec) const
{
    bool hitAnything = false;

    double closest = ray.tMax;

    HitRecord tempRec;

    for (const auto& tri : m_triangles)
    {
        if (tri.intersect(ray, tempRec))
        {
            if (tempRec.t < closest)
            {
                closest = tempRec.t;
                rec = tempRec;
                hitAnything = true;
            }
        }
    }

    return hitAnything;
}