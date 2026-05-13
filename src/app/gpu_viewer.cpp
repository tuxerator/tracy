#include "app/gpu_viewer.h"
#include <QDateTime>
#include <QDir>
#include <chrono>

GpuViewer::GpuViewer(int width,
                     int height,
                     const Scene& scene,
                     const Camera& camera,
                     const Integrator& integrator,
                     int samplesPerPixel,
                     QLabel* durationLabel,
                     QWidget* parent)
    : QOpenGLWidget(parent),
      m_width(width),
      m_height(height),
      m_scene(scene),
      m_camera(camera),
      m_integrator(integrator),
      m_samplesPerPixel(samplesPerPixel),
      m_durationLabel(durationLabel) {
    setMinimumSize(width, height);
    m_renderer = new GpuRenderer();
}

GpuViewer::~GpuViewer() {
    makeCurrent();
    delete m_renderer;
    m_renderer = nullptr;
    doneCurrent();
}

const QImage& GpuViewer::getImage() const {
    return m_dummyImage;
}

void GpuViewer::stopRaytrace() {}

void GpuViewer::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    m_renderer->initialize();
    m_renderer->uploadScene(m_scene);
}

void GpuViewer::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

void GpuViewer::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto start = std::chrono::high_resolution_clock::now();
    m_renderer->render(m_camera);
    auto end = std::chrono::high_resolution_clock::now();

    auto ms = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0;
    if (m_durationLabel) {
        m_durationLabel->setText(QString("GPU: %1 ms").arg(ms, 0, 'f', 1));
    }
}
