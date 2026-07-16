#include "shading/materials/glass_material.h"

#include <algorithm>
#include <cmath>
#include <random>

#include <glm/glm.hpp>

namespace {

// Fresnel-Reflektanz-Näherung nach Schlick.
double schlickReflectance(double cosine, double refractionRatio) {
  double r0 = (1.0 - refractionRatio) / (1.0 + refractionRatio);
  r0 = r0 * r0;
  return r0 + (1.0 - r0) * std::pow(1.0 - cosine, 5.0);
}

// Einfacher Thread-lokaler Zufallszahlengenerator für die
// Reflexion/Brechung-Entscheidung. Falls dein Projekt schon einen
// zentralen RNG hat (z.B. für Pfadverfolgung/Sampling), ersetz das
// hier gerne dadurch, um Konsistenz mit deinem Sampling-Seed zu wahren.
double randomDouble() {
  thread_local std::mt19937 generator(std::random_device{}());
  thread_local std::uniform_real_distribution<double> dist(0.0, 1.0);
  return dist(generator);
}

} // namespace

GlassMaterial::GlassMaterial(const Color &tint, double ior)
    : m_tint(tint), m_ior(ior) {}

Color GlassMaterial::albedo(const HitRecord &) const {
  return m_tint;
}

Color GlassMaterial::evaluate(const HitRecord &, const glm::dvec3 &,
                              const glm::dvec3 &) const {
  // Wie beim Spiegel: Glas ist rein spekular (Delta-Funktion), daher
  // trägt es nichts zur "normalen" BRDF-Auswertung bei.
  return Color(0.0, 0.0, 0.0);
}

MaterialSample GlassMaterial::sample(const HitRecord &rec,
                                     const glm::dvec3 &wo) const {
  // wo zeigt von der Oberfläche weg (zur Kamera/vorherigem Pfadsegment).
  // Der tatsächlich einfallende Strahl zeigt in die Gegenrichtung.
  glm::dvec3 incident = glm::normalize(-wo);

  // rec.frontFace: true, wenn der Strahl von außen auf die Fläche trifft
  // (Luft -> Glas), false wenn von innen (Glas -> Luft). Falls dein
  glm::dvec3 outwardNormal =
      rec.frontFace ? rec.shadingNormal : -rec.shadingNormal;
  double refractionRatio = rec.frontFace ? (1.0 / m_ior) : m_ior;

  double cosTheta = std::min(glm::dot(-incident, outwardNormal), 1.0);
  double sinTheta = std::sqrt(std::max(0.0, 1.0 - cosTheta * cosTheta));

  bool cannotRefract = refractionRatio * sinTheta > 1.0;
  double reflectance = schlickReflectance(cosTheta, refractionRatio);

  glm::dvec3 direction;
  if (cannotRefract || reflectance > randomDouble()) {
    // Totalreflexion oder per Fresnel-Wahrscheinlichkeit reflektiert
    direction = glm::reflect(incident, outwardNormal);
  } else {
    direction = glm::refract(incident, outwardNormal, refractionRatio);
  }

  MaterialSample s;
  s.wi = direction;
  s.weight = m_tint;
  s.pdf = 1.0;
  s.specular = true;
  return s;
}
