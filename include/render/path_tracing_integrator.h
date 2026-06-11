// Stub for later Monte Carlo global illumination work.

#pragma once

#include "render/integrator.h"

class PathTracingIntegrator : public Integrator {
    public:
        PathTracingIntegrator(int maxDepth = 8,
                            const Color& background = Color(0.0, 0.0, 0.0));

        Color Li(const Ray& ray, const Scene& scene, int depth = 0) const;

    private:
        int   m_maxDepth;
        Color m_background;
};
