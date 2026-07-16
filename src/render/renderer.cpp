// Renderer implementation. Samples each pixel m_samplesPerPixel times with
// random jitter for anti-aliasing, then averages the result.
//
// Parallelized over image rows with OpenMP: pixels/samples are independent
// of one another, so this is embarrassingly parallel. Requires randomDouble()
// to use thread-local RNG state (see note below / core/random.h).

#include "render/renderer.h"

#include "core/random.h"
#include "io/image.h"
#include "render/integrator.h"
#include "scene/camera.h"
#include "scene/scene.h"
#include "iostream"

Renderer::Renderer(int samplesPerPixel)
    : m_samplesPerPixel(samplesPerPixel > 0 ? samplesPerPixel : 1) {}

void Renderer::render(const Scene &scene, const Camera &camera,
                      const Integrator &integrator, Image &image) const {
  const int width = image.width();
  const int height = image.height();

  // schedule(dynamic) instead of the default static split: pixels that hit
  // more bounces / more lights cost more, so dynamic chunks balance load
  // better across threads than a fixed row range per thread.
#pragma omp parallel for schedule(dynamic, 1)
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      Color accumulated(0.0);
      for (int s = 0; s < m_samplesPerPixel; ++s) {
        double sx = x + randomDouble();
        double sy = y + randomDouble();
        Ray ray = camera.generateRay(sx, sy);
        accumulated += integrator.Li(ray, scene, 0);
      }
      // Each thread only ever writes to pixels from its own (x, y), so this
      // is safe as long as Image::setPixel doesn't touch shared mutable
      // state beyond the target pixel (e.g. no shared running totals).
      image.setPixel(x, y,
                     accumulated / static_cast<double>(m_samplesPerPixel));
    }
    std::cout << "done wth " << y << " out of " << height << std::endl; 
  }
}
