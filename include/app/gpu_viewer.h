#pragma once

#include <QLabel>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <chrono>

#include "app/iviewer.h"
#include "render/gpu_renderer.h"
#include "render/integrator.h"
#include "scene/camera.h"
#include "scene/scene.h"

class GpuViewer : public QOpenGLWidget,
                  protected QOpenGLFunctions,
                  public IViewer {
public:
  GpuViewer(const Scene &scene, const Camera &camera,
            const Integrator &integrator, int samplesPerPixel,
            QLabel *durationLabel, QWidget *parent = nullptr);

  ~GpuViewer() override;

  // IViewer implementation
  const QImage &getImage() const override;
  void stopRaytrace() override;
  void render() override;
  QWidget *asWidget() override { return this; }

  void setSamplesPerPixel(const int samplesPerPixel) override {
    m_samplesPerPixel = samplesPerPixel;
  }

protected:
  void initializeGL() override;
  void paintGL() override;

private:
  const Scene &m_scene;
  const Camera &m_camera;
  const Integrator &m_integrator;
  int m_samplesPerPixel = 1;

  int m_currentSample = 0;
  int m_batchSize = 1;
  bool m_renderInProgress = false;
  std::chrono::time_point<std::chrono::high_resolution_clock> m_renderStartTime;

  QLabel *m_durationLabel = nullptr;
  QImage m_dummyImage;

  GpuRenderer *m_renderer = nullptr;
};
