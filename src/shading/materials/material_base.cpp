#include "shading/materials/material_base.h"

#include <glm/glm.hpp>

MaterialSample Material::sample(const HitRecord &, const glm::dvec3 &) const {
  return MaterialSample{};
}

double Material::pdf(const HitRecord &, const glm::dvec3 &,
                     const glm::dvec3 &) const {
  return 0.0;
}

GPUMaterial Material::toGPU() const {
  GPUMaterial gpu{};
  gpu.type = static_cast<int>(type());
  Color e = emission(HitRecord{});
  gpu.emission = glm::vec4(e.x, e.y, e.z, 0.0f);
  return gpu;
}
