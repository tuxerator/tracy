#include "io/obj_loader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "geometry/triangle.h"
#include "scene/scene.h"

namespace {
struct OBJVertex {
  int vertexIndex = -1;
  int uvIndex = -1;
};

OBJVertex parseOBJVertex(const std::string &token) {

  OBJVertex result;

  size_t firstSlash = token.find('/');
  size_t secondSlash = token.find('/', firstSlash + 1);

  if (firstSlash == std::string::npos) {
    result.vertexIndex = std::stoi(token) - 1;

    return result;
  }

  result.vertexIndex = std::stoi(token.substr(0, firstSlash)) - 1;

  // uv index exists
  if (secondSlash > firstSlash + 1) {
    result.uvIndex =
        std::stoi(token.substr(firstSlash + 1, secondSlash - firstSlash - 1)) -
        1;
  } else {
    std::string uvPart = token.substr(firstSlash + 1);

    if (!uvPart.empty()) {
      result.uvIndex = std::stoi(uvPart) - 1;
    }
  }

  return result;
}
} // namespace

void OBJLoader::load(const std::string &path, Scene &scene,
                     std::shared_ptr<Material> material) {
  std::ifstream file(path);

  if (!file.is_open()) {
    std::cout << "Failed to open OBJ: " << path << std::endl;
    return;
  }

  std::vector<glm::dvec3> vertices;
  std::vector<glm::dvec2> uvs;

  std::string line;

  int triangleCount = 0;

  while (std::getline(file, line)) {
    std::stringstream ss(line);

    std::string prefix;
    ss >> prefix;

    if (prefix == "v") {
      double x, y, z;
      ss >> x >> y >> z;

      glm::dvec3 p(x, y, z);

      // Optional transform
      p *= 0.03;
      p += glm::dvec3(0.0, 3.0, -5.0);

      vertices.push_back(p);
    }

    else if (prefix == "vt") {
      double u, v;
      ss >> u >> v;

      // Flip V because image origin
      // is usually top-left
      v = 1.0 - v;

      uvs.push_back(glm::dvec2(u, v));
    }

    else if (prefix == "f") {
      std::vector<OBJVertex> faceVertices;

      std::string token;

      while (ss >> token) {
        faceVertices.push_back(parseOBJVertex(token));
      }

      if (faceVertices.size() < 3) {
        continue;
      }

      OBJVertex v0 = faceVertices[0];

      for (size_t k = 1; k + 1 < faceVertices.size(); ++k) {
        OBJVertex v1 = faceVertices[k];
        OBJVertex v2 = faceVertices[k + 1];

        // UV fallback
        glm::dvec2 uv0(0.0);
        glm::dvec2 uv1(0.0);
        glm::dvec2 uv2(0.0);

        if (v0.uvIndex >= 0)
          uv0 = uvs[v0.uvIndex];

        if (v1.uvIndex >= 0)
          uv1 = uvs[v1.uvIndex];

        if (v2.uvIndex >= 0)
          uv2 = uvs[v2.uvIndex];

        scene.addPrimitive(std::make_shared<Triangle>(vertices[v0.vertexIndex],
                                                      vertices[v1.vertexIndex],
                                                      vertices[v2.vertexIndex],

                                                      uv0, uv1, uv2,

                                                      material));

        triangleCount++;
      }
    }
  }

  std::cout << "Loaded OBJ: " << triangleCount << " triangles" << std::endl;
}
