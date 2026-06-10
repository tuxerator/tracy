// Minimal bounding box implementation.

#include "accel/bbox.h"

BBox::BBox()
    : m_min(0.0, 0.0, 0.0), m_max(0.0, 0.0, 0.0) {}

BBox::BBox(const glm::dvec3& minCorner, const glm::dvec3& maxCorner)
    : m_min(minCorner), m_max(maxCorner) {}

bool BBox::intersect(const Ray& ray) const
{
    double tmin = ray.tMin;
    double tmax = ray.tMax;

    for (int axis = 0; axis < 3; axis++)
    {
        double invD = 1.0 / ray.direction[axis];

        double t0 =
            (m_min[axis] - ray.origin[axis]) * invD;

        double t1 =
            (m_max[axis] - ray.origin[axis]) * invD;

        if (invD < 0.0)
            std::swap(t0, t1);

        tmin = std::max(tmin, t0);
        tmax = std::min(tmax, t1);

        if (tmax <= tmin)
            return false;
    }

    return true;
}

glm::dvec3 BBox::centroid() const {
    return (m_min + m_max) * 0.5;
}