// Direct illumination integrator with hard shadows.

#include "render/direct_lighting_integrator.h"

#include <glm/glm.hpp>
#include <algorithm>
#include <memory>

#include "scene/scene.h"
#include "shading/light.h"
#include "shading/material.h"
#include "scene/hit_record.h"
#include "core/constants.h"

DirectLightingIntegrator::DirectLightingIntegrator(int maxDepth,
                                                   const Color& background)
    : m_maxDepth(maxDepth), m_background(background) {}

Color DirectLightingIntegrator::Li(const Ray& ray,
                                   const Scene& scene,
                                   int depth) const {
    if (depth > m_maxDepth) {
        return Color(0.0, 0.0, 0.0);
    }

    HitRecord rec;

    // If the ray misses the scene, return the background color.
    if (!scene.intersect(ray, rec)) {
        return m_background;
    }

    Color result = rec.material ? rec.material->emission(rec) : Color(0.0, 0.0, 0.0);

    // wo is the outgoing direction toward the camera/viewer.
    glm::dvec3 wo = -ray.direction;

    // Traverse all lights in the scene.
    for (const auto& lightBase : scene.lights()) {
        auto pointLight = std::dynamic_pointer_cast<PointLight>(lightBase);
        if (!pointLight) {
            continue;
        }

        glm::dvec3 lightPos = pointLight->position();
        glm::dvec3 wiVec = lightPos - rec.position;

        double distanceToLight = glm::length(wiVec);
        glm::dvec3 wi = wiVec / distanceToLight;

        Ray shadowRay;
        shadowRay.origin = rec.position + rec.geometricNormal * 1e-6;
        shadowRay.direction = wi;

        HitRecord shadowRec;

        if (scene.intersect(shadowRay, shadowRec)) {
            if (shadowRec.t < distanceToLight) {
                 continue;
            }
        }

        Color f = rec.material->evaluate(rec, wo, wi);

        Color Li = pointLight->intensity() / (distanceToLight * distanceToLight);

        double cosTheta = std::max(0.0, glm::dot(rec.shadingNormal, wi));

        result += f * Li * cosTheta;
    }

    return result;
}
