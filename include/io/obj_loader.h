#pragma once

#include <string>
#include "shading/material.h"

class Scene;

class OBJLoader {
public:
    static void load(const std::string &path, Scene &scene,
                  std::shared_ptr<Material> material,
                  const glm::dvec3 &position = glm::dvec3(0.0),
                  const glm::dvec3 &rotationDegrees = glm::dvec3(0.0),
                  double scale = 1.0,
                  bool placeOnFloor = false,
                  double floorY = 0.0);
};