#include "shading/materials/emissive_material.h"

EmissiveMaterial::EmissiveMaterial(const Color &color, double intensity)
    : m_color(color), m_intensity(intensity) {}

Color EmissiveMaterial::albedo(const HitRecord &) const {
  return Color(0.0, 0.0, 0.0);  // Emissive reflektiert nichts
}

Color EmissiveMaterial::emission(const HitRecord &) const {
  return m_color * m_intensity;
}

Color EmissiveMaterial::evaluate(const HitRecord &, const glm::dvec3 &,
                                 const glm::dvec3 &) const {
  return Color(0.0, 0.0, 0.0);  // kein reflektierter Anteil
}
