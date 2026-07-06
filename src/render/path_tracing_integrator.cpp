// Starter stub for a later path tracer.
// Returning black keeps the project buildable before the module is implemented.

#include "render/path_tracing_integrator.h"
#include "scene/scene.h"
#include "core/random.h"
#include <iostream>

PathTracingIntegrator::PathTracingIntegrator(int maxDepth)
    : m_maxDepth(maxDepth) {}

static int debugPixel = 0;

Color PathTracingIntegrator::Li(const Ray &ray, const Scene &scene,
                                int depth) const {
  if (depth > m_maxDepth)
    return Color(0.0, 0.0, 0.0);

  HitRecord rec;
  if (!scene.intersect(ray, rec)) {
    return scene.background();
  }

  // Emission bei direktem Treffer (Kamera-Sicht auf die Lichtkugel selbst)
  Color result =
      rec.material ? rec.material->emission(rec) : Color(0.0, 0.0, 0.0);

  glm::dvec3 wo = -ray.direction;

  MaterialSample ms = rec.material->sample(rec, wo);

  if (!ms.specular) {
    for (const auto &lightBase : scene.lights()) {
      auto pointLight = std::dynamic_pointer_cast<PointLight>(lightBase);
      if (!pointLight)
        continue;

      glm::dvec3 wiVec = pointLight->position() - rec.position;
      double dist = glm::length(wiVec);
      glm::dvec3 wi = wiVec / dist;

      Ray shadowRay;
      shadowRay.origin = rec.position + rec.geometricNormal * 1e-6;
      shadowRay.direction = wi;

      HitRecord shadowRec;
      if (scene.intersect(shadowRay, shadowRec) && shadowRec.t < dist)
        continue;

      Color f = rec.material->evaluate(rec, wo, wi);
      Color Li = pointLight->intensity() / (dist * dist);
      double cosTheta = std::max(0.0, glm::dot(rec.shadingNormal, wi));
      result += f * Li * cosTheta;
    }

    // --- NEU: Emissive Primitives als Flächenlichter (Next Event Estimation) ---
    for (const auto &emissivePrim : scene.emissivePrimitives()) {
      // Verhindert, dass eine Kugel sich selbst beleuchtet
      if (emissivePrim->material() == rec.material)
        continue;

      double u1 = randomDouble();
      double u2 = randomDouble();
      LightSample ls = emissivePrim->sampleLi(rec.position, u1, u2);

      if (ls.pdf <= 0.0)
        continue;

      glm::dvec3 wiVec = ls.position - rec.position;
      double dist2 = glm::dot(wiVec, wiVec);
      double dist = std::sqrt(dist2);
      if (dist < 1e-8)
        continue;
      glm::dvec3 wi = wiVec / dist;

      double cosTheta = std::max(0.0, glm::dot(rec.shadingNormal, wi));
      if (cosTheta <= 0.0)
        continue;

      Ray shadowRay;
      shadowRay.origin = rec.position + rec.geometricNormal * 1e-6;
      shadowRay.direction = wi;

      HitRecord shadowRec;
      // Etwas Toleranz (1e-4) vor der Lichtquelle, damit die Kugel sich nicht
      // selbst verschattet (Rundungsfehler an der Oberfläche)
      if (scene.intersect(shadowRay, shadowRec) && shadowRec.t < dist - 1e-4)
        continue;

      HitRecord lightRec;
      lightRec.position = ls.position;
      Color Le = emissivePrim->material()->emission(lightRec);

      Color f = rec.material->evaluate(rec, wo, wi);
      result += f * Le * cosTheta / ls.pdf;
    }
  }

  // --- Indirekter / spekularer Strahl (unverändert) ---
  if (ms.pdf > 0.0) {
    Ray bounceRay;
    bounceRay.origin = rec.position + rec.geometricNormal * 1e-6;
    bounceRay.direction = ms.wi;

    Color incoming = Li(bounceRay, scene, depth + 1);

    if (ms.specular) {
      result += ms.weight * incoming;
    } else {
      double cosTheta = std::max(0.0, glm::dot(rec.shadingNormal, ms.wi));
      result += ms.weight * incoming * cosTheta / ms.pdf;
    }
  }

  return result;
}
