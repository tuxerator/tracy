#ifndef TRIANGLE_GLSL
#define TRIANGLE_GLSL

#include "src/core/shader/ray.glsl"
#include "src/scene/shader/hit_record.glsl"

struct Triangle {
  vec4 a;
  vec4 b;
  vec4 c;
  int materialIndex;
};

bool triangle_intersect(Triangle triangle, Ray ray, out HitRecord rec) {
  vec3 edge1 = triangle.b.xyz - triangle.a.xyz;
  vec3 edge2 = triangle.c.xyz - triangle.a.xyz;

  vec3 pvec = cross(ray.direction, edge2);
  float det = dot(edge1, pvec);

  if (abs(det) < kEpsilon) {
    return false;
  }

  float invDet = 1.0 / det;
  vec3 tvec = ray.origin - triangle.a.xyz;

  float u = dot(tvec, pvec) * invDet;
  if (u < 0.0 || u > 1.0) {
    return false;
  }

  vec3 qvec = cross(tvec, edge1);
  float v = dot(ray.direction, qvec) * invDet;
  if (v < 0.0 || u + v > 1.0) {
    return false;
  }

  float t = dot(edge2, qvec) * invDet;
  if (t < ray.tMin || t > ray.tMax) {
    return false;
  }

  rec.t = t;
  rec.position = ray_at(ray, t);
  rec.materialIndex = triangle.materialIndex;

  vec3 outwardNormal = normalize(cross(edge1, edge2));
  hitRecord_setFaceNormal(rec, ray.direction, outwardNormal);

  return true;
}

#endif
