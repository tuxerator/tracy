#pragma once

#include "core/color.h"
#include "shading/materials/material_base.h"

class EmissiveMaterial : public Material {
public:
  EmissiveMaterial(const Color &color, double intensity = 1.0);

  MaterialType type() const override { return MaterialType::Emissive; }

  Color albedo(const HitRecord &) const override;
  Color emission(const HitRecord &rec) const override;
  Color evaluate(const HitRecord &, const glm::dvec3 &,
                 const glm::dvec3 &) const override;

private:
  Color m_color;
  double m_intensity;
};
