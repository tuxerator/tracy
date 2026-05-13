#ifndef SCENE_GLSL
#define SCENE_GLSL

#include "src/core/shader/ray.glsl"
#include "src/scene/shader/hit_record.glsl"
#include "src/geometry/shader/sphere.glsl"
#include "src/geometry/shader/triangle.glsl"
#include "src/geometry/shader/plane.glsl"
#include "src/shading/shader/material.glsl"
#include "src/shading/shader/light.glsl"

layout(std430, binding = 0) readonly buffer SphereBuffer {
  Sphere spheres[];
};
layout(std430, binding = 1) readonly buffer TriangleBuffer {
  Triangle triangles[];
};
layout(std430, binding = 2) readonly buffer PlaneBuffer {
  Plane planes[];
};
layout(std430, binding = 3) readonly buffer MaterialBuffer {
  Material materials[];
};
layout(std430, binding = 4) readonly buffer LightBuffer {
  PointLight lights[];
};
layout(std430, binding = 5) readonly buffer BackgroundBuffer {
  vec3 background;
};

uniform int u_numSpheres;
uniform int u_numTriangles;
uniform int u_numPlanes;
uniform int u_numLights;

bool scene_intersect(Ray ray, out HitRecord rec) {
  bool hit = false;
  float closest = ray.tMax;
  HitRecord temp;

  for (int i = 0; i < u_numSpheres; i++) {
    Sphere s = spheres[i];
    if (sphere_intersect(s, ray, temp)) {
      if (temp.t < closest && temp.t > ray.tMin) {
        closest = temp.t;
        rec = temp;
        rec.materialIndex = s.materialIndex;
        hit = true;
      }
    }
  }

  for (int i = 0; i < u_numTriangles; i++) {
    Triangle t = triangles[i];
    if (triangle_intersect(t, ray, temp)) {
      if (temp.t < closest && temp.t > ray.tMin) {
        closest = temp.t;
        rec = temp;
        rec.materialIndex = t.materialIndex;
        hit = true;
      }
    }
  }

  for (int i = 0; i < u_numPlanes; i++) {
    Plane p = planes[i];
    if (plane_intersect(p, ray, temp)) {
      if (temp.t < closest && temp.t > ray.tMin) {
        closest = temp.t;
        rec = temp;
        rec.materialIndex = p.materialIndex;
        hit = true;
      }
    }
  }

  return hit;
}

bool scene_occluded(Ray ray) {
  HitRecord temp;
  return scene_intersect(ray, temp);
}

#endif
