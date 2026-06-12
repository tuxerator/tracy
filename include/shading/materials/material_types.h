#pragma once

#include <glm/glm.hpp>

#include "core/color.h"

enum class MaterialType { Lambert, Mirror, Dielectric, Emissive, Unknown };

struct MaterialSample {
  glm::dvec3 wi{0.0, 1.0, 0.0};
  Color weight{0.0, 0.0, 0.0};
  double pdf = 0.0;
  bool valid = false;
  bool delta = false;
  bool specular = false;
};

struct GPUMaterial {
    int type;
    int _pad0[3];
    glm::vec4 albedo;
    glm::vec4 emission;
};
static_assert(sizeof(GPUMaterial) == 48);
