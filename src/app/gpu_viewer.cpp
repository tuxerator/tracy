#include "app/gpu_viewer.h"
#include <QDateTime>
#include <QDir>
#include <QTimer>
#include <chrono>
#include <qlogging.h>
#include <qobjectdefs.h>

GpuViewer::GpuViewer(const Scene &scene, const Camera &camera,
                     const Integrator &integrator, int samplesPerPixel,
                     QLabel *durationLabel, QWidget *parent)
    : QOpenGLWidget(parent), m_scene(scene), m_camera(camera),
      m_integrator(integrator), m_samplesPerPixel(samplesPerPixel),
      m_durationLabel(durationLabel) {
  resize(m_camera.imageWidth(), m_camera.imageHeight());
  m_renderer = new GpuRenderer(m_camera);
}

GpuViewer::~GpuViewer() {
  makeCurrent();
  delete m_renderer;
  m_renderer = nullptr;
  doneCurrent();
}

const QImage &GpuViewer::getImage() const { return m_dummyImage; }

void GpuViewer::stopRaytrace() {
  if (m_renderInProgress) {
    m_renderer->endRender();
    m_renderInProgress = false;
  }
}

void GpuViewer::render() {
  resize(m_camera.imageWidth(), m_camera.imageHeight());
  if (m_renderInProgress) {
    m_renderer->endRender();
    m_renderInProgress = false;
  }
  m_renderer->beginRender();
  m_currentSample = 0;
  m_renderInProgress = true;
  m_renderStartTime = std::chrono::high_resolution_clock::now();
  if (m_durationLabel) {
    m_durationLabel->setText("Rendering...");
  }
  update();
}

void GpuViewer::initializeGL() {
  initializeOpenGLFunctions();
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  m_renderer->initialize();
  m_renderer->uploadScene(m_scene);
}

void GpuViewer::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (m_renderInProgress) {
    int batchEnd = std::min(m_currentSample + m_batchSize, m_samplesPerPixel);
    for (int s = m_currentSample + 1; s <= batchEnd; s++) {
      m_renderer->dispatchSample(s);
    }
    m_currentSample = batchEnd;

    if (m_currentSample >= m_samplesPerPixel) {
      m_renderer->endRender();
      m_renderInProgress = false;
      auto end = std::chrono::high_resolution_clock::now();
      auto ms = std::chrono::duration_cast<std::chrono::microseconds>(
                    end - m_renderStartTime)
                    .count() /
                1000.0;
      if (m_durationLabel) {
        m_durationLabel->setText(QString("GPU: %1 ms").arg(ms, 0, 'f', 1));
      }
    }
    QTimer::singleShot(0, this, [this]() { update(); });
  }

  int displaySamples = m_currentSample > 0 ? m_currentSample : 1;
  m_renderer->display(displaySamples);
}
