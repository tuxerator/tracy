#pragma once

#include <string>
#include "shading/material.h"

class Scene;

class OBJLoader {
public:
    static void load(const std::string& path,
                     Scene& scene,
                     std::shared_ptr<Material> material);
};