#ifndef DIRECT_LIGHTING_INTEGRATOR_GLSL
#define DIRECT_LIGHTING_INTEGRATOR_GLSL

#include "src/scene/shader/scene.glsl"

const int kMaxDepth = 3;

vec3 direct_lighting_Li(Ray ray, int depth) {
  if (depth > kMaxDepth) {
    return vec3(0.0);
  }

  HitRecord rec;
  if (!scene_intersect(ray, rec)) {
    return background;
  }

  Material mat = materials[rec.materialIndex];
  vec3 result = mat.emission.rgb;

  vec3 wo = -ray.direction;

  for (int i = 0; i < u_numLights; i++) {
    PointLight light = lights[i];
    vec3 lightPos = light.position.xyz;
    vec3 wiVec = lightPos - rec.position;
    float dist = length(wiVec);
    vec3 wi = wiVec / dist;

    float cosTheta = dot(rec.normal, wi);
    if (cosTheta <= 0.0) {
      continue;
    }

    Ray shadowRay;
    shadowRay.origin = rec.position + rec.normal * kEpsilon;
    shadowRay.direction = wi;
    shadowRay.tMin = kEpsilon;
    shadowRay.tMax = kInfinity;

    HitRecord shadowRec;
    if (scene_intersect(shadowRay, shadowRec)) {
      if (shadowRec.t < dist) {
        continue;
      }
    }

    vec3 f = mat.albedo.rgb / 3.14159265;

    result += f;
  }

  return result;
}

#endif
