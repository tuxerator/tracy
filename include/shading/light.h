// Light sources seen by the integrator.

#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "core/color.h"

struct GPUPointLight {
    glm::vec4 position;
    glm::vec4 intensity;
};
static_assert(sizeof(GPUPointLight) == 32);

class Light {
public:
    virtual ~Light() = default;
};

class PointLight : public Light {
public:
    PointLight(const glm::dvec3& position,
               const Color& intensityColor,
               double intensity);

    // World-space light position.
    const glm::dvec3& position() const;

    // Scalar intensity multiplied by the light color.
    Color intensity() const;

    // GPU representation for compute shader upload.
    GPUPointLight toGPU() const;

private:
    glm::dvec3 m_position;
    Color m_intensityColor;
    double m_intensity;
};
