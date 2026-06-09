// Material implementation for the Global Illumination starter.

#include "shading/material.h"

#include <glm/glm.hpp>

#include "core/constants.h"

MaterialSample Material::sample(const HitRecord &, const glm::dvec3 &) const {
  // Default placeholder implementation.
  // Later modules can override this with cosine-weighted hemisphere sampling,
  // specular reflection sampling, refraction sampling, etc.
  return MaterialSample{};
}

double Material::pdf(const HitRecord &, const glm::dvec3 &,
                     const glm::dvec3 &) const {
  // Default placeholder implementation.
  return 0.0;
}

LambertMaterial::LambertMaterial(const Color &color)
    : m_texture(std::make_shared<ConstantTexture>(color)) {}

LambertMaterial::LambertMaterial(std::shared_ptr<Texture> texture)
    : m_texture(std::move(texture)) {}

Color LambertMaterial::albedo(const HitRecord &rec) const {
  return m_texture->value(rec.uv, rec.position);
}

Color LambertMaterial::evaluate(const HitRecord &rec, const glm::dvec3 &,
                                const glm::dvec3 &wi) const {
  double cosTheta = glm::dot(rec.shadingNormal, wi);
  if (cosTheta <= 0.0) {
    return Color(0.0, 0.0, 0.0);
  }

  return albedo(rec) / constants::kPi;
}

GPUMaterial Material::toGPU() const {
  GPUMaterial gpu{};
  gpu.type = static_cast<int>(type());
  Color e = emission(HitRecord{});
  gpu.emission = glm::vec4(e.x, e.y, e.z, 0.0f);
  return gpu;
}

GPUMaterial LambertMaterial::toGPU() const {
  GPUMaterial gpu = Material::toGPU();
  Color a = albedo(HitRecord{});
  gpu.albedo = glm::vec4(a.x, a.y, a.z, 0.0f);
  return gpu;
}

