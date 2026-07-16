#pragma once

#include "core/color.h"
#include "shading/materials/material_base.h"

// Dielektrisches Glas-Material: reflektiert und bricht Licht nach
// Snell'schem Gesetz. Der Anteil Reflexion vs. Brechung wird pro Sample
// stochastisch über die Fresnel-Reflektanz (Schlick-Näherung) entschieden,
// analog zum "Dielectric"-Material aus Ray Tracing in One Weekend.
class GlassMaterial : public Material {
public:
  // tint: Einfärbung des durchgelassenen/reflektierten Lichts
  // ior:  Brechungsindex (z.B. 1.5 für Fensterglas, 2.4 für Diamant)
  GlassMaterial(const Color &tint = Color(1.0, 1.0, 1.0), double ior = 1.5);

  MaterialType type() const override { return MaterialType::Glass; }

  Color albedo(const HitRecord &) const override;
  Color evaluate(const HitRecord &, const glm::dvec3 &wo,
                 const glm::dvec3 &wi) const override;
  MaterialSample sample(const HitRecord &,
                        const glm::dvec3 &wo) const override;

private:
  Color m_tint;
  double m_ior;
};
