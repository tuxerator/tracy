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
        glm::dvec3(1.5, -4.9, 3.0), 0.5, blueLight));

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

  auto ceilingLight = std::make_shared<EmissiveMaterial>(Color(1.0, 0.95, 0.1), 3.0);
    scene.addEmissivePrimitive(std::make_shared<Box>(
        glm::dvec3(-2.0, 5.0, -2.0),   // Ecke
        glm::dvec3(4.0, 0.0, 0.0),     // u-Kante (Breite 4 in x)
        glm::dvec3(0.0, 0.0, 4.0),     // v-Kante (Tiefe 4 in z)
        glm::dvec3(0.0, 0.4, 0.0),   // w-Kante (Dicke 0.05 nach unten in y)
        ceilingLight));

    scene.addLight(std::make_shared<PointLight>(
        glm::dvec3(5.0, 5.0, 5.0),
        Color(1.0, 1.0, 1.0),
        50.0));

  scene.buildBVH();

  Camera camera(
      glm::dvec3(15.0, 0.5, 3.0),
      glm::dvec3(11.5, 0.5, 2.5), glm::dvec3(0.0, 1.0, 0.0), 45.0, width,
      height);

  return {scene, camera};
}

SceneSetup SceneFactory::createRoomScene(int width, int height) {
  Scene scene(Color(0.02, 0.02, 0.02));

  // Dicke der "Rechtecke": sehr dünne Boxen, die nach außen aus dem Raum
  // herausragen, damit der Innenraum (x:[-1,1], y:[0,2], z:[2,4]) frei bleibt.
  const double eps = 0.02;

  auto white   = std::make_shared<LambertMaterial>(Color(0.70, 0.85, 0.70));
  auto mirror  = std::make_shared<MirrorMaterial>(Color(0.95, 0.95, 0.95));
  auto ceilingLight = std::make_shared<EmissiveMaterial>(Color(1.0, 1.0, 1.0), 4.0);

  // Boden: y = 0, Dicke nach unten
  scene.addPrimitive(std::make_shared<Box>(
      glm::dvec3(-1.0, -eps, 2.0),
      glm::dvec3(2.0, 0.0, 0.0),   // Breite in x
      glm::dvec3(0.0, 0.0, 2.0),   // Tiefe in z
      glm::dvec3(0.0, eps, 0.0),   // Dicke nach oben bis y=0
      white));

  // Decke: y = 2, Dicke nach oben, emissiv (weißes Licht)
  scene.addEmissivePrimitive(std::make_shared<Box>(
      glm::dvec3(-1.0, 2.0, 2.0),
      glm::dvec3(2.0, 0.0, 0.0),   // Breite in x
      glm::dvec3(0.0, 0.0, 2.0),   // Tiefe in z
      glm::dvec3(0.0, eps, 0.0),   // Dicke nach oben
      ceilingLight));

  // Linke Wand (von der Kamera aus, x = -1): Spiegel
  scene.addPrimitive(std::make_shared<Box>(
      glm::dvec3(-1.0 - eps, 0.0, 2.0),
      glm::dvec3(0.0, 2.0, 0.0),   // Höhe in y
      glm::dvec3(0.0, 0.0, 2.0),   // Tiefe in z
      glm::dvec3(eps, 0.0, 0.0),   // Dicke nach innen bis x=-1
      mirror));

  // Rechte Wand: x = 1
  scene.addPrimitive(std::make_shared<Box>(
      glm::dvec3(1.0, 0.0, 2.0),
      glm::dvec3(0.0, 2.0, 0.0),   // Höhe in y
      glm::dvec3(0.0, 0.0, 2.0),   // Tiefe in z
      glm::dvec3(eps, 0.0, 0.0),   // Dicke nach außen
      white));

  // Rückwand: z = 4 (am weitesten von der Kamera entfernt)
  scene.addPrimitive(std::make_shared<Box>(
      glm::dvec3(-1.0, 0.0, 4.0),
      glm::dvec3(2.0, 0.0, 0.0),   // Breite in x
      glm::dvec3(0.0, 2.0, 0.0),   // Höhe in y
      glm::dvec3(0.0, 0.0, eps),   // Dicke nach außen
      white));


  auto dolphinTexturePath = QString(PROJECT_SOURCE_DIR) + "/assets/models/10014_dolphin_v1_Diffuse.jpg";
  auto dolphinTexture = std::make_shared<ImageTexture>(dolphinTexturePath);
  auto dolphinMaterial = std::make_shared<LambertMaterial>(dolphinTexture);

  auto dolphinObjPath = QString(PROJECT_SOURCE_DIR) + "/assets/models/10014_dolphin_v2_max2011_it2.obj";

OBJLoader::load(dolphinObjPath.toStdString(), scene, dolphinMaterial,
                 glm::dvec3(0.0, 0.0, 3.0),
                 glm::dvec3(-110.0, 90.0, 0.0),
                 0.0040,
                 true,
                 0.0);

  scene.buildBVH();

    Camera camera(
        glm::dvec3(0.0, 1.0, 1.2),
        glm::dvec3(0.0, 1.0, 5.0),
        glm::dvec3(0.0, 1.0, 0.0),
        95.0, width, height);

  return {scene, camera};
}
