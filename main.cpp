// Entry point for the minimal Global Illumination starter project.
// This version builds a reusable preset scene through SceneFactory.

#include <QApplication>

#include "app/gui.h"
#include "app/scene_factory.h"
#include "render/direct_lighting_integrator.h"

int main(int argc, char **argv) {
  QApplication app(argc, argv);

  const int width = 600;
  const int height = 800;
  const int samplesPerPixel = 16;

  SceneSetup setup = SceneFactory::createStarterScene(width, height);

  DirectLightingIntegrator integrator(3);

  Gui window(width, height, setup.scene, setup.camera, integrator,
             samplesPerPixel);
  window.show();

  return app.exec();
}
