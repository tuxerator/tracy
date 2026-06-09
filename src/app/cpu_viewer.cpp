// Viewer implementation for the minimal starter.
// The render is done once in the constructor, and the resulting QImage is
// drawn.

#include "app/cpu_viewer.h"

#include <QPainter>
#include <QResizeEvent>
#include <chrono>

#include "io/image.h"
#include "render/renderer.h"

CpuViewer::CpuViewer(const Scene &scene, const Camera &camera,
                     const Integrator &integrator, int samplesPerPixel,
                     QLabel *durationLabel, QWidget *parent)
    : QWidget(parent), m_scene(scene), m_camera(camera),
      m_integrator(integrator), m_samplesPerPixel(samplesPerPixel),
      m_durationLabel(durationLabel),
      m_image(m_camera.imageWidth(), m_camera.imageHeight(),
              QImage::Format_RGB32) {
  renderScene();
}

void CpuViewer::renderScene() {
  resize(m_camera.imageWidth(), m_camera.imageHeight());

  auto start = std::chrono::high_resolution_clock::now();

  Image image(m_camera.imageWidth(), m_camera.imageHeight());
  Renderer renderer(m_samplesPerPixel);
  renderer.render(m_scene, m_camera, m_integrator, image);

  m_image = image.qimage();

  auto end = std::chrono::high_resolution_clock::now();
  auto durationMs =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
          .count();

  if (m_durationLabel) {
    m_durationLabel->setText(QString("Render time: %1 ms (%2 spp)")
                                 .arg(durationMs)
                                 .arg(m_samplesPerPixel));
  }

  update();
}

const QImage &CpuViewer::getImage() const { return m_image; }

void CpuViewer::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);
  renderScene();
}

void CpuViewer::stopRaytrace() {
  // No background rendering thread is used in the minimal starter.
  // This method is kept for interface compatibility and future extension.
}

void CpuViewer::paintEvent(QPaintEvent *) {
  QPainter painter(this);
  painter.fillRect(rect(), Qt::black);

  if (!m_image.isNull()) {
    painter.drawImage(rect(), m_image);
  }
}

QSize CpuViewer::sizeHint() const {
  return QSize(m_camera.imageWidth(), m_camera.imageHeight());
}
