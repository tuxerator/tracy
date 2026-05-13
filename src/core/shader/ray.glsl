#ifndef RAY_GLSL
#define RAY_GLSL

const float kEpsilon = 1e-6;
const float kInfinity = 1e30;

struct Ray {
  vec3 origin;
  vec3 direction;
  float tMin;
  float tMax;
};

Ray ray_create(vec3 o, vec3 d, float minT, float maxT) {
  Ray r;
  r.origin = o;
  r.direction = d;
  r.tMin = minT;
  r.tMax = maxT;
  return r;
}

Ray ray_create_default() {
  return ray_create(vec3(0.0), vec3(0.0, 0.0, -1.0), kEpsilon, kInfinity);
}

vec3 ray_at(Ray r, float t) {
  return r.origin + t * r.direction;
}

#endif
