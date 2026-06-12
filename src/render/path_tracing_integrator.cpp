// Starter stub for a later path tracer.
// Returning black keeps the project buildable before the module is implemented.

#include "render/path_tracing_integrator.h"
#include "scene/scene.h"
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

  // Emission (z.B. Lichtquellen als Geometrie)
  Color result =
      rec.material ? rec.material->emission(rec) : Color(0.0, 0.0, 0.0);

  glm::dvec3 wo = -ray.direction;

  // --- Direkte Beleuchtung (nur für nicht-spekulare Materialien) ---
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
  }

  // --- Indirekter / spekularer Strahl ---
  if (ms.pdf > 0.0) {
    Ray bounceRay;
    bounceRay.origin = rec.position + rec.geometricNormal * 1e-6;
    bounceRay.direction = ms.wi;

    Color incoming = Li(bounceRay, scene, depth + 1);

    if (ms.specular) {
      // Spiegel: weight enthält alles, kein cos/pdf nötig
      result += ms.weight * incoming;
    } else {
      // Diffus: standard Monte-Carlo Schätzer
      double cosTheta = std::max(0.0, glm::dot(rec.shadingNormal, ms.wi));
      result += ms.weight * incoming * cosTheta / ms.pdf;
    }
  }

  return result;
}
