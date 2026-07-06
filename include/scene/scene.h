// Scene stores the current primitives and lights and answers ray queries.

#pragma once

#include <memory>
#include <vector>

#include "geometry/primitive.h"
#include "shading/light.h"
#include "geometry/triangle.h"
#include "accel/bvh.h"

class Scene {
public:
  Scene(Color background);
  void addPrimitive(const std::shared_ptr<Primitive> &primitive);
  void addLight(const std::shared_ptr<Light> &light);

  // Find the closest hit, if any.
  bool intersect(const Ray &ray, HitRecord &rec) const;

  // Return true as soon as any primitive blocks the ray.
  bool occluded(const Ray &ray) const;

    void buildBVH();

    const Color &background() const { return m_background; }

    const std::vector<std::shared_ptr<Light>> &lights() const;
    const std::vector<std::shared_ptr<Primitive>> &primitives() const {
        return m_primitives;
    }

    void addEmissivePrimitive(std::shared_ptr<Primitive> prim);
    const std::vector<std::shared_ptr<Primitive>>& emissivePrimitives() const;

private:
    Color m_background;
    std::vector<std::shared_ptr<Primitive>> m_primitives;
    std::shared_ptr<Primitive> m_root;
    std::vector<std::shared_ptr<Light>> m_lights;
    std::vector<std::shared_ptr<Primitive>> m_emissivePrimitives;

  // Later directions may cache acceleration structures here.
};
