// Scene implementation using brute-force traversal over all primitives.

#include "scene/scene.h"
#include "core/color.h"

Scene::Scene(Color background) : m_background(background) {}

void Scene::addPrimitive(const std::shared_ptr<Primitive> &primitive) {
  m_primitives.push_back(primitive);
}

void Scene::addLight(const std::shared_ptr<Light> &light) {
  m_lights.push_back(light);
}

bool Scene::intersect(const Ray &ray, HitRecord &rec) const {
  bool hitAnything = false;
  double closestSoFar = ray.tMax;

  for (const auto &primitive : m_primitives) {
    HitRecord tempRec;

    if (primitive->intersect(ray, tempRec)) {
      if (tempRec.t < closestSoFar && tempRec.t > ray.tMin) {
        hitAnything = true;
        closestSoFar = tempRec.t;
        rec = tempRec;
      }
    }
  }

  return hitAnything;
}

bool Scene::occluded(const Ray &ray) const {
  HitRecord rec;
  return intersect(ray, rec);
}

const std::vector<std::shared_ptr<Light>> &Scene::lights() const {
  return m_lights;
}
