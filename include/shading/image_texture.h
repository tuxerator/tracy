#pragma once

#include <QImage>
#include "shading/texture.h"

class ImageTexture : public Texture {
public:
    explicit ImageTexture(const QString& filename);

    Color value(const glm::dvec2& uv,
                const glm::dvec3& position) const override;

private:
    QImage m_image;
};