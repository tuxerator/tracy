// Scene implementation using brute-force traversal over all primitives.

#include "scene/scene.h"
#include "core/color.h"
#include "geometry/triangle.h"

#include <iostream>

Scene::Scene(Color background) : m_background(background) {}

void Scene::addPrimitive(const std::shared_ptr<Primitive> &primitive) {
  m_primitives.push_back(primitive);
}

void Scene::addLight(const std::shared_ptr<Light> &light) {
  m_lights.push_back(light);
}

bool Scene::intersect(
    const Ray& ray,
    HitRecord& rec) const
{
    if (!m_root)
    {
        return false;
    }

    return m_root->intersect(ray, rec);
}

void Scene::buildBVH()
{
    std::cout << "Building BVH with " << m_primitives.size() << " primitives" << std::endl;

    if (m_primitives.empty())
    {
        return;
    }

    m_root = std::make_shared<BVHNode>(
        m_primitives,
        0,
        m_primitives.size());
}

bool Scene::occluded(const Ray &ray) const {
  HitRecord rec;
  return intersect(ray, rec);
}

const std::vector<std::shared_ptr<Light>>& Scene::lights() const {
    return m_lights;
}

void Scene::addEmissivePrimitive(std::shared_ptr<Primitive> prim) {
    m_emissivePrimitives.push_back(prim);
    addPrimitive(prim); // trotzdem normal zur BVH hinzufügen
}

const std::vector<std::shared_ptr<Primitive>>& Scene::emissivePrimitives() const {
    return m_emissivePrimitives;
}
