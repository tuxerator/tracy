// Implementation of reusable scene presets for the starter project.

#include "app/scene_factory.h"

#include <memory>

#include "geometry/geometry.h"
#include "io/obj_loader.h"
#include "shading/image_texture.h"
#include "shading/shading.h"

SceneSetup SceneFactory::createStarterScene(int width, int height) {
  Scene scene(Color(0.08, 0.08, 0.80));

    auto red   = std::make_shared<LambertMaterial>(Color(0.8, 0.2, 0.2));
    auto green = std::make_shared<LambertMaterial>(Color(0.2, 0.8, 0.2));
    auto gray  = std::make_shared<LambertMaterial>(Color(0.7, 0.7, 0.7));
    auto mirror = std::make_shared<MirrorMaterial>(Color(0.95, 0.95, 0.95));
    // Blaue Lichtkugel
    auto blueLight = std::make_shared<EmissiveMaterial>(Color(0.2, 0.4, 0.9), 1.0);
    scene.addEmissivePrimitive(std::make_shared<Sphere>(
        glm::dvec3(2.5, -3.0, 4.8), 0.5, blueLight));

    // Grüne Lichtkuge
    auto greenLight = std::make_shared<EmissiveMaterial>(Color(0.2, 0.9, 0.3), 1.0);
    scene.addEmissivePrimitive(std::make_shared<Sphere>(
        glm::dvec3(-1.0, 3.0, 1.0), 0.3, greenLight));

  auto dolphinTexturePath = QString(PROJECT_SOURCE_DIR) +
                            "/assets/models/10014_dolphin_v1_Diffuse.jpg";
  auto dolphinTexture = std::make_shared<ImageTexture>(dolphinTexturePath);

  auto dolphinMaterial = std::make_shared<LambertMaterial>(dolphinTexture);

  scene.addPrimitive(
      std::make_shared<Sphere>(glm::dvec3(0.5, -3.0, 3.0), 1.4, red));

  scene.addPrimitive(
      std::make_shared<Sphere>(glm::dvec3(-1.5, 0.5, 1.5), 2.0, mirror));

  auto dolphinObjPath = QString(PROJECT_SOURCE_DIR) +
                        "/assets/models/10014_dolphin_v2_max2011_it2.obj";
  OBJLoader::load(dolphinObjPath.toStdString(), scene, dolphinMaterial);

    scene.addLight(std::make_shared<PointLight>(
        glm::dvec3(5.0, 5.0, 5.0),
        Color(1.0, 1.0, 1.0),
        50.0));

  scene.buildBVH();

  Camera camera(
      glm::dvec3(15.0, 0.5, 3.0), // eye (40 Meter nach rechts verschoben)
      glm::dvec3(11.5, 0.5, 2.5), glm::dvec3(0.0, 1.0, 0.0), 45.0, width,
      height);

  return {scene, camera};
}
