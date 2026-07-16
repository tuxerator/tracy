#include "shading/materials/lambert_material.h"

#include <glm/glm.hpp>

#include "core/constants.h"

#include "core/random.h"

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

GPUMaterial LambertMaterial::toGPU() const {
  GPUMaterial gpu = Material::toGPU();
  Color a = albedo(HitRecord{});
  gpu.albedo = glm::vec4(a.x, a.y, a.z, 0.0f);
  return gpu;
}

void buildOrthonormalBasis(const glm::dvec3& n,
                           glm::dvec3& tangent,
                           glm::dvec3& bitangent)
{
    if (std::abs(n.x) > 0.9)
        tangent = glm::normalize(glm::cross(glm::dvec3(0,1,0), n));
    else
        tangent = glm::normalize(glm::cross(glm::dvec3(1,0,0), n));

    bitangent = glm::cross(n, tangent);
}

MaterialSample LambertMaterial::sample(const HitRecord &rec,
                                       const glm::dvec3 &) const
{
    MaterialSample s;

    // Zufällige Richtung im lokalen Koordinatensystem
    glm::dvec3 local =randomCosineDirection();

    // Tangentenbasis erzeugen
    glm::dvec3 tangent, bitangent;
    buildOrthonormalBasis(rec.shadingNormal,
                          tangent,
                          bitangent);

    // Von lokal nach Weltkoordinaten transformieren
    s.wi =
        local.x * tangent +
        local.y * bitangent +
        local.z * rec.shadingNormal;

    s.wi = glm::normalize(s.wi);

    s.weight = albedo(rec) / constants::kPi;

    double cosTheta =
        std::max(0.0,
                 glm::dot(rec.shadingNormal,
                          s.wi));

    s.pdf = cosTheta / constants::kPi;

    s.specular = false;

    return s;
}