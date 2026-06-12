#pragma once

#include <memory>

#include "core/color.h"
#include "shading/materials/material_base.h"
#include "shading/texture.h"

class LambertMaterial : public Material {
public:
  explicit LambertMaterial(const Color &color);
  explicit LambertMaterial(std::shared_ptr<Texture> texture);

  MaterialType type() const override { return MaterialType::Lambert; }

  Color albedo(const HitRecord &rec) const override;
  Color evaluate(const HitRecord &rec, const glm::dvec3 &wo,
                 const glm::dvec3 &wi) const override;

  GPUMaterial toGPU() const override;

private:
  std::shared_ptr<Texture> m_texture;
};
