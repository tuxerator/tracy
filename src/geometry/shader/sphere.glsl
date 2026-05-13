#ifndef SPHERE_GLSL
#define SPHERE_GLSL

#include "src/core/shader/ray.glsl"
#include "src/scene/shader/hit_record.glsl"

struct Sphere {
  vec4 center;
  int materialIndex;
};

bool sphere_intersect(Sphere s, Ray ray, out HitRecord rec) {
  vec3 oc = ray.origin - s.center.xyz;

  // Solve dot(ray.direction, ray.direction) * t² + 2 * dot(ray.direction,
  // ray.origin) * t + dot(ray.origin, ray.origin) - r² = 0
  float a = dot(ray.direction, ray.direction);
  float b = dot(ray.direction, oc);
  float c = dot(oc, oc) - pow(s.center.w, 2);
  float discriminant = b * b - a * c;

  bool hit = discriminant >= 0;

  if (hit) {
    float rootDiscrim = sqrt(discriminant);
    float q = -(b + sign(b) * rootDiscrim);
    float t0 = q / a;
    float t1 = c / q;

    if (t0 > t1) {
      float tmp = t0;
      t0 = t1;
      t1 = tmp;
    }

    rec.t = t0;
    rec.materialIndex = s.materialIndex;
    rec.position = ray_at(ray, t0);
    hitRecord_setFaceNormal(rec, ray.direction, normalize(rec.position - s.center.xyz));
  }

  return hit;
}

#endif
