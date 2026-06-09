// Widget that triggers one render and displays the resulting image.

#pragma once

#include <QImage>
#include <QLabel>
#include <QWidget>

#include "app/iviewer.h"
#include "render/integrator.h"
#include "scene/camera.h"
#include "scene/scene.h"

class CpuViewer : public QWidget, public IViewer {
public:
  CpuViewer(int width, int height, const Scene &scene, const Camera &camera,
            const Integrator &integrator, int samplesPerPixel,
            QLabel *durationLabel, QWidget *parent = nullptr);

  const QImage &getImage() const override;

  // Kept for compatibility with future threaded/progressive viewers.
  void stopRaytrace() override;

  void render() override { renderScene(); }

  QWidget *asWidget() override { return this; }

  void setSamplesPerPixel(const int samplesPerPixel) override {
    m_samplesPerPixel = samplesPerPixel;
  }

protected:
  void paintEvent(QPaintEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;
  QSize sizeHint() const override;

  // Render once into m_image.
  void renderScene();

private:
  int m_width = 0;
  int m_height = 0;

  const Scene &m_scene;
  const Camera &m_camera;
  const Integrator &m_integrator;
  int m_samplesPerPixel = 1;

  QLabel *m_durationLabel = nullptr;
  QImage m_image;
};
