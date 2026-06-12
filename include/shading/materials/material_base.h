#pragma once

#include <glm/glm.hpp>

#include "scene/hit_record.h"
#include "shading/materials/material_types.h"

class Material {
public:
  virtual ~Material() = default;

  virtual MaterialType type() const { return MaterialType::Unknown; }

  virtual Color albedo(const HitRecord &rec) const = 0;

  virtual Color emission(const HitRecord &) const {
    return Color(0.0, 0.0, 0.0);
  }

  virtual Color evaluate(const HitRecord &rec, const glm::dvec3 &wo,
                         const glm::dvec3 &wi) const = 0;

  virtual MaterialSample sample(const HitRecord &rec,
                                const glm::dvec3 &wo) const;

  virtual double pdf(const HitRecord &rec, const glm::dvec3 &wo,
                     const glm::dvec3 &wi) const;

  virtual GPUMaterial toGPU() const;

  virtual bool isDelta() const { return false; }
};
