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

  // Rekursion beenden, damit Strahlen nicht unendlich oft reflektiert werden.
  if (depth > m_maxDepth)
    return Color(0.0, 0.0, 0.0);

  HitRecord rec;

  // Prüfen, ob der Strahl überhaupt ein Objekt trifft.
  // Falls nicht, wird die Hintergrundfarbe zurückgegeben.
  if (!scene.intersect(ray, rec)) {
    return scene.background();
  }

  // Falls das getroffene Objekt selbst Licht emittiert (z.B. eine Lichtkugel),
  // wird dessen Emission direkt zur Ergebnisfarbe hinzugefügt.
  Color result =
      rec.material ? rec.material->emission(rec) : Color(0.0, 0.0, 0.0);

  // wo = Richtung vom Oberflächenpunkt zurück zur Kamera.
  // Die Ray-Richtung zeigt von der Kamera weg, daher das Minuszeichen.
  glm::dvec3 wo = -ray.direction;

  // Das Material bestimmt einen neuen Strahl für die indirekte Beleuchtung.
  // Gleichzeitig liefert es Gewicht, PDF und ob es sich um eine Spiegelreflexion handelt.
  MaterialSample ms = rec.material->sample(rec, wo);

  // Direkte Beleuchtung wird nur für nicht-spiegelnde Materialien berechnet.
  // Spiegel erhalten ihre Beleuchtung ausschließlich über den Reflexionsstrahl.
  if (!ms.specular) {

    // =========================
    // Direkte Beleuchtung durch Punktlichter
    // =========================
    for (const auto &lightBase : scene.lights()) {

      // Nur PointLights berücksichtigen.
      auto pointLight = std::dynamic_pointer_cast<PointLight>(lightBase);
      if (!pointLight)
        continue;

      // Richtung vom Oberflächenpunkt zum Licht.
      glm::dvec3 wiVec = pointLight->position() - rec.position;

      // Abstand zum Licht.
      double dist = glm::length(wiVec);

      // Normierte Lichtrichtung.
      glm::dvec3 wi = wiVec / dist;

      // Schattenstrahl leicht über der Oberfläche starten,
      // damit der Strahl nicht sofort dieselbe Fläche trifft.
      Ray shadowRay;
      shadowRay.origin = rec.position + rec.geometricNormal * 1e-6;
      shadowRay.direction = wi;

      HitRecord shadowRec;

      // Liegt ein Objekt zwischen Punkt und Licht?
      // Dann befindet sich der Punkt im Schatten.
      if (scene.intersect(shadowRay, shadowRec) && shadowRec.t < dist)
        continue;

      // BRDF des Materials auswerten.
      Color f = rec.material->evaluate(rec, wo, wi);

      // Lichtintensität nimmt quadratisch mit dem Abstand ab.
      Color Li = pointLight->intensity() / (dist * dist);

      // Lambert-Kosinus-Term.
      double cosTheta = std::max(0.0, glm::dot(rec.shadingNormal, wi));

      // Beitrag des Punktlichts aufsummieren.
      result += f * Li * cosTheta;
    }

    // =========================
    // Direkte Beleuchtung durch Flächenlichter
    // =========================
    for (const auto &emissivePrim : scene.emissivePrimitives()) {

      // Verhindert, dass sich eine Lichtquelle selbst beleuchtet.
      if (emissivePrim->material() == rec.material)
        continue;

      // Zufälligen Punkt auf der Lichtquelle sampeln.
      double u1 = randomDouble();
      double u2 = randomDouble();
      LightSample ls = emissivePrim->sampleLi(rec.position, u1, u2);

      // Ungültiges Sample überspringen.
      if (ls.pdf <= 0.0)
        continue;

      // Richtung und Abstand zum gesampelten Lichtpunkt.
      glm::dvec3 wiVec = ls.position - rec.position;
      double dist2 = glm::dot(wiVec, wiVec);
      double dist = std::sqrt(dist2);

      // Numerische Probleme vermeiden.
      if (dist < 1e-8)
        continue;

      glm::dvec3 wi = wiVec / dist;

      // Oberfläche muss zum Licht zeigen.
      double cosTheta = std::max(0.0, glm::dot(rec.shadingNormal, wi));
      if (cosTheta <= 0.0)
        continue;

      // Schattenstrahl zum gesampelten Punkt der Lichtquelle.
      Ray shadowRay;
      shadowRay.origin = rec.position + rec.geometricNormal * 1e-6;
      shadowRay.direction = wi;

      HitRecord shadowRec;

      // Prüfen, ob ein Objekt die Sicht auf die Lichtquelle blockiert.
      // Kleine Toleranz verhindert Selbstverschattung der Lichtquelle.
      if (scene.intersect(shadowRay, shadowRec) && shadowRec.t < dist - 1e-4)
        continue;

      // Emission der Lichtquelle bestimmen.
      HitRecord lightRec;
      lightRec.position = ls.position;
      Color Le = emissivePrim->material()->emission(lightRec);

      // BRDF des Materials auswerten.
      Color f = rec.material->evaluate(rec, wo, wi);

      // Monte-Carlo-Schätzer:
      // BRDF * Emission * cos(theta) / PDF
      result += f * Le * cosTheta / ls.pdf;
    }
  }

  // =========================
  // Indirekte Beleuchtung (Path Tracing)
  // =========================
  // Russian Roulette
  if (ms.pdf > 0.0) {

      // Ab einer bestimmten Tiefe beginnen wir,
      // unwichtige Pfade zufällig abzubrechen.
      if (depth > 3) {

          // Wahrscheinlichkeit, dass der Pfad weiterläuft.
          double continueProbability = 0.8;

          if (randomDouble() > continueProbability) {
              return result;
          }

          // Energie korrigieren:
          // Wenn wir nur 80% der Pfade behalten,
          // müssen diese 80% entsprechend stärker gewichtet werden.
          ms.weight /= continueProbability;
      }


      Ray bounceRay;
      bounceRay.origin = rec.position + rec.geometricNormal * 1e-6;
      bounceRay.direction = ms.wi;

      Color incoming = Li(bounceRay, scene, depth + 1);


      if (ms.specular) {
          result += ms.weight * incoming;

      } else {
          double cosTheta =
              std::max(0.0,
                      glm::dot(rec.shadingNormal,
                                ms.wi));

          result += ms.weight *
                    incoming *
                    cosTheta /
                    ms.pdf;
      }
  }

  // Gesamte Radiance (Emission + direkte + indirekte Beleuchtung)
  return result;
}
