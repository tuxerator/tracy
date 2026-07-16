// Minimal random utility implementations.
// The starter keeps these helpers ready so later stochastic rendering tasks
// can add sampling without reshaping the whole project.

#include "core/random.h"
#include <glm/glm.hpp>
#include <cmath>

#include <random>

namespace {

thread_local std::mt19937 rng(std::random_device{}());

}  // namespace

double randomDouble() {
    static thread_local std::uniform_real_distribution<double> distribution(0.0, 1.0);
    return distribution(rng);
}

double randomDouble(double minValue, double maxValue) {
    return minValue + (maxValue - minValue) * randomDouble();
}

glm::dvec3 randomCosineDirection()
{
    double u1 = randomDouble();
    double u2 = randomDouble();

    double r = std::sqrt(u1);
    double theta = 2.0 * M_PI * u2;

    double x = r * std::cos(theta);
    double y = r * std::sin(theta);
    double z = std::sqrt(1.0 - u1);

    return glm::dvec3(x, y, z);
}
