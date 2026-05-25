#include "io/obj_loader.h"

#include <fstream>
#include <sstream>
#include <vector>

#include "geometry/triangle.h"
#include "scene/scene.h"

void OBJLoader::load(const std::string& path,
                     Scene& scene,
                     std::shared_ptr<Material> material)
{
    std::ifstream file(path);

    if (!file.is_open())
        return;

    std::vector<glm::dvec3> vertices;

    std::string line;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);

        std::string prefix;
        ss >> prefix;

        if (prefix == "v")
        {
            double x, y, z;
            ss >> x >> y >> z;

            vertices.push_back(glm::dvec3(x, y, z));
        }
        else if (prefix == "f")
        {
            int i0, i1, i2;

            ss >> i0 >> i1 >> i2;

            i0--;
            i1--;
            i2--;

            scene.addPrimitive(
                std::make_shared<Triangle>(
                    vertices[i0],
                    vertices[i1],
                    vertices[i2],
                    material
                )
            );
        }
    }
}
