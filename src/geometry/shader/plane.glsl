#ifndef PLANE_GLSL
#define PLANE_GLSL

#include "src/core/shader/ray.glsl"
#include "src/scene/shader/hit_record.glsl"

struct Plane {
  vec4 point;
  vec4 normal;
  int materialIndex;
};

bool plane_intersect(Plane p, Ray ray, out HitRecord rec) {
  float denom = dot(p.normal.xyz, ray.direction);

  if (abs(denom) < kEpsilon) {
    return false;
  }

  float t = dot(p.point.xyz - ray.origin, p.normal.xyz) / denom;
  if (t < ray.tMin || t > ray.tMax) {
    return false;
  }

  rec.t = t;
  rec.position = ray_at(ray, t);
  rec.materialIndex = p.materialIndex;
  hitRecord_setFaceNormal(rec, ray.direction, p.normal.xyz);

  return true;
}

#endif
