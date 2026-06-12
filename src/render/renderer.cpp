// Renderer implementation. Samples each pixel m_samplesPerPixel times with
// random jitter for anti-aliasing, then averages the result.

#include "render/renderer.h"

#include "core/random.h"
#include "io/image.h"
#include "render/integrator.h"
#include "scene/camera.h"
#include "scene/scene.h"

Renderer::Renderer(int samplesPerPixel)
    : m_samplesPerPixel(samplesPerPixel > 0 ? samplesPerPixel : 1) {}

void Renderer::render(const Scene &scene, const Camera &camera,
                      const Integrator &integrator, Image &image) const {
  for (int y = 0; y < image.height(); ++y) {
    for (int x = 0; x < image.width(); ++x) {
      Color accumulated(0.0);
      for (int s = 0; s < m_samplesPerPixel; ++s) {
        double sx = x + randomDouble();
        double sy = y + randomDouble();
        Ray ray = camera.generateRay(sx, sy);
        accumulated += integrator.Li(ray, scene, 0);
      }
      image.setPixel(x, y,
                     accumulated / static_cast<double>(m_samplesPerPixel));
    }
  }
}
