// Simple pinhole camera used to generate primary rays.

#pragma once

#include <format>
#include <glm/glm.hpp>

#include "core/ray.h"

class Camera {
public:
    Camera(const glm::dvec3& eye,
           const glm::dvec3& target,
           const glm::dvec3& up,
           double verticalFovDegrees,
           int imageWidth,
           int imageHeight);

    // Generate a ray through an arbitrary sample location inside the image.
    Ray generateRay(double sampleX, double sampleY) const;

    // Convenience overload for the pixel center.
    Ray generateRay(int px, int py) const;

    void resize(int width, int height);

    int imageWidth() const { return m_imageWidth; }
    int imageHeight() const { return m_imageHeight; }

    glm::dvec3 origin() const { return m_eye; }
    glm::dvec3 direction() const { return m_forward; }
    glm::dvec3 right() const { return m_right; }
    glm::dvec3 up() const { return m_up; }
    double halfWidth() const { return m_halfWidth; }
    double halfHeight() const { return m_halfHeight; }

private:
    glm::dvec3 m_eye;
    glm::dvec3 m_forward;
    glm::dvec3 m_right;
    glm::dvec3 m_up;

    double m_halfHeight;
    double m_halfWidth;

    int m_imageWidth;
    int m_imageHeight;
};

template <>
struct std::formatter<Camera> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const Camera& cam, std::format_context& ctx) const {
        return std::format_to(ctx.out(),
            "Camera(eye=({}, {}, {}), forward=({}, {}, {}), image={}x{})",
            cam.origin().x, cam.origin().y, cam.origin().z,
            cam.direction().x, cam.direction().y, cam.direction().z,
            cam.imageWidth(), cam.imageHeight());
    }
};
