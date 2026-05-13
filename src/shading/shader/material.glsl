#ifndef MATERIAL_GLSL
#define MATERIAL_GLSL

const int MAT_LAMBERT = 0;
const int MAT_MIRROR = 1;
const int MAT_DIELECTRIC = 2;
const int MAT_EMISSIVE = 3;
const int MAT_UNKNOWN = 4;

struct Material {
  int type;
  vec4 albedo;
  vec4 emission;
};

#endif
