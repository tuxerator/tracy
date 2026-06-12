#pragma once

#include "core/color.h"
#include "shading/materials/material_base.h"

class MirrorMaterial : public Material {
public:
  MirrorMaterial(const Color &tint = Color(1.0, 1.0, 1.0));

  MaterialType type() const override { return MaterialType::Mirror; }

  Color albedo(const HitRecord &) const override;
  Color evaluate(const HitRecord &, const glm::dvec3 &wo,
                 const glm::dvec3 &wi) const override;
  MaterialSample sample(const HitRecord &,
                        const glm::dvec3 &wo) const override;

private:
  Color m_tint;
};
