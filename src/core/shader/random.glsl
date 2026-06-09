#ifndef RANDOM_GLSL
#define RANDOM_GLSL

uint pcg_hash(uint v) {
  uint state = v * 747796405u + 2891336453u;
  uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
  return (word >> 22u) ^ word;
}

float rand_float(uint seed) {
  return float(pcg_hash(seed)) / 4294967296.0;
}

uint rand_seed(int coord, int smp) {
  return uint(coord) ^ (uint(smp) << 16u);
}

vec2 rand_pixel(ivec2 pixel, int smp) {
  return vec2(
    rand_float(rand_seed(pixel.x, smp)),
    rand_float(rand_seed(pixel.y, smp) + 1u)
  );
}

#endif
