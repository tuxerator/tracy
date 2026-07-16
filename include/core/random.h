// Minimal random utility declarations.
// Extend this file when adding stochastic sampling or path tracing.

#pragma once

#include <glm/glm.hpp>

double randomDouble();
double randomDouble(double minValue, double maxValue);
glm::dvec3 randomCosineDirection();