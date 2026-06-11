#include "shading/image_texture.h"

#include <QColor>
#include <algorithm>
#include <QDebug>

namespace
{
    double clamp01(double x)
    {
        return std::max(0.0,
               std::min(1.0, x));
    }
}

ImageTexture::ImageTexture(
    const QString& filename
)
    : m_image(filename)
{
    if (m_image.isNull())
    {
        qWarning() << "Failed to load texture:"
                   << filename;
        return;
    }

    qDebug() << "Loaded texture:"
             << filename
             << "("
             << m_image.width()
             << "x"
             << m_image.height()
             << ")";

    m_image =
        m_image.convertToFormat(
            QImage::Format_RGB32
        );
}

Color ImageTexture::value(
    const glm::dvec2& uv,
    const glm::dvec3&
) const
{
    double u = clamp01(uv.x);
    double v = clamp01(uv.y);

    int x =
        static_cast<int>(
            u * (m_image.width() - 1)
        );

    int y =
        static_cast<int>(
            v * (m_image.height() - 1)
        );

    QColor c =
        m_image.pixelColor(x, y);

    return Color(
        c.redF(),
        c.greenF(),
        c.blueF()
    );
}