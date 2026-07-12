#include "io/obj_loader.h"

#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>

#include "geometry/triangle.h"
#include "scene/scene.h"

namespace {
struct OBJVertex {
  int vertexIndex = -1;
  int uvIndex = -1;
};

struct FaceData {
  std::vector<OBJVertex> vertices;
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
                      std::shared_ptr<Material> material,
                      const glm::dvec3 &position,
                      const glm::dvec3 &rotationDegrees,
                      double scale,
                      bool placeOnFloor,
                      double floorY) {
  std::ifstream file(path);

  if (!file.is_open()) {
    std::cout << "Failed to open OBJ: " << path << std::endl;
    return;
  }

  // --- Pass 1: Datei roh einlesen (keine Transformation) ---
  std::vector<glm::dvec3> rawVertices;
  std::vector<glm::dvec2> uvs;
  std::vector<FaceData> faces;

  std::string line;

  while (std::getline(file, line)) {
    std::stringstream ss(line);

    std::string prefix;
    ss >> prefix;

    if (prefix == "v") {
      double x, y, z;
      ss >> x >> y >> z;
      rawVertices.emplace_back(x, y, z);
    }

    else if (prefix == "vt") {
      double u, v;
      ss >> u >> v;

      // Flip V because image origin
      // is usually top-left
      v = 1.0 - v;

      uvs.emplace_back(u, v);
    }

    else if (prefix == "f") {
      FaceData face;
      std::string token;

      while (ss >> token) {
        face.vertices.push_back(parseOBJVertex(token));
      }

      if (face.vertices.size() >= 3) {
        faces.push_back(std::move(face));
      }
    }
  }

  {
    glm::dvec3 rawMin(std::numeric_limits<double>::max());
    glm::dvec3 rawMax(std::numeric_limits<double>::lowest());
    for (const glm::dvec3 &raw : rawVertices) {
      rawMin = glm::min(rawMin, raw);
      rawMax = glm::max(rawMax, raw);
    }
    glm::dvec3 rawSize = rawMax - rawMin;
    std::cout << "OBJ raw bounding box size: (" << rawSize.x << ", "
              << rawSize.y << ", " << rawSize.z << ")" << std::endl;
  }

  // --- Transform aufbauen: skalieren -> rotieren (X, dann Y, dann Z) ---
  glm::dvec3 rotRad = glm::radians(rotationDegrees);

  glm::dmat4 rotation(1.0);
  rotation = glm::rotate(rotation, rotRad.z, glm::dvec3(0.0, 0.0, 1.0));
  rotation = glm::rotate(rotation, rotRad.y, glm::dvec3(0.0, 1.0, 0.0));
  rotation = glm::rotate(rotation, rotRad.x, glm::dvec3(1.0, 0.0, 0.0));

  std::vector<glm::dvec3> vertices;
  vertices.reserve(rawVertices.size());

  double minY = std::numeric_limits<double>::max();

  for (const glm::dvec3 &raw : rawVertices) {
    glm::dvec3 scaled = raw * scale;
    glm::dvec3 rotated = glm::dvec3(rotation * glm::dvec4(scaled, 1.0));
    vertices.push_back(rotated);
    minY = std::min(minY, rotated.y);
  }

  // Falls placeOnFloor gesetzt ist: zusätzlich so verschieben, dass der
  // tiefste Punkt des (skalierten/rotierten) Objekts genau auf floorY landet.
  glm::dvec3 finalOffset = position;
  if (placeOnFloor && !vertices.empty()) {
    finalOffset.y += floorY - minY;
  }

  for (glm::dvec3 &v : vertices) {
    v += finalOffset;
  }

  // --- Debug: finale Bounding Box nach Skalierung/Rotation/Position ---
  {
    glm::dvec3 finalMin(std::numeric_limits<double>::max());
    glm::dvec3 finalMax(std::numeric_limits<double>::lowest());
    for (const glm::dvec3 &v : vertices) {
      finalMin = glm::min(finalMin, v);
      finalMax = glm::max(finalMax, v);
    }
    std::cout << "OBJ final bounding box: min(" << finalMin.x << ", "
              << finalMin.y << ", " << finalMin.z << ") max(" << finalMax.x
              << ", " << finalMax.y << ", " << finalMax.z << ")" << std::endl;
  }

  // --- Pass 2: Dreiecke aus den transformierten Vertices bauen ---
  int triangleCount = 0;

  for (const FaceData &face : faces) {
    const OBJVertex &v0 = face.vertices[0];

    for (size_t k = 1; k + 1 < face.vertices.size(); ++k) {
      const OBJVertex &v1 = face.vertices[k];
      const OBJVertex &v2 = face.vertices[k + 1];

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

  std::cout << "Loaded OBJ: " << triangleCount << " triangles" << std::endl;
}
