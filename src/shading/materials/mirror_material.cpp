#include "shading/materials/mirror_material.h"

#include <glm/glm.hpp>

MirrorMaterial::MirrorMaterial(const Color &tint) : m_tint(tint) {}

Color MirrorMaterial::albedo(const HitRecord &) const {
  return m_tint;
}

Color MirrorMaterial::evaluate(const HitRecord &, const glm::dvec3 &,
                               const glm::dvec3 &) const {
  return Color(0.0, 0.0, 0.0);
}

MaterialSample MirrorMaterial::sample(const HitRecord &rec,
                                      const glm::dvec3 &wo) const {
  glm::dvec3 wi = glm::reflect(-wo, rec.shadingNormal);

  MaterialSample s;
  s.wi = wi;
  s.weight = m_tint;
  s.pdf = 1.0;
  s.specular = true;
  return s;
}
