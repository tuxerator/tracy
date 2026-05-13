#pragma once

#include <QLabel>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>

#include "app/iviewer.h"
#include "render/gpu_renderer.h"
#include "render/integrator.h"
#include "scene/camera.h"
#include "scene/scene.h"

class GpuViewer : public QOpenGLWidget,
                  protected QOpenGLFunctions,
                  public IViewer {
public:
  GpuViewer(int width, int height, const Scene &scene, const Camera &camera,
            const Integrator &integrator, int samplesPerPixel,
            QLabel *durationLabel, QWidget *parent = nullptr);

  ~GpuViewer() override;

  // IViewer implementation
  const QImage &getImage() const override;
  void stopRaytrace() override;
  void render() override { update(); }
  QWidget *asWidget() override { return this; }

protected:
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;

private:
  int m_width = 0;
  int m_height = 0;

  const Scene &m_scene;
  const Camera &m_camera;
  const Integrator &m_integrator;
  int m_samplesPerPixel = 1;

  QLabel *m_durationLabel = nullptr;
  QImage m_dummyImage;

  GpuRenderer *m_renderer = nullptr;
};
