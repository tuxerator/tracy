// Point light implementation.

#include "shading/light.h"

PointLight::PointLight(const glm::dvec3& position,
                       const Color& intensityColor,
                       double intensity)
    : m_position(position),
      m_intensityColor(intensityColor),
      m_intensity(intensity) {}

const glm::dvec3& PointLight::position() const {
    return m_position;
}

Color PointLight::intensity() const {
    return m_intensity * m_intensityColor;
}

GPUPointLight PointLight::toGPU() const {
    GPUPointLight gpu{};
    gpu.position = glm::vec4(m_position.x, m_position.y, m_position.z, 0.0f);
    Color c = m_intensity * m_intensityColor;
    gpu.intensity = glm::vec4(c.x, c.y, c.z, 0.0f);
    return gpu;
}
