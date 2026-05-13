// Main application window: toolbar + rendered image view.

#pragma once

#include <QComboBox>
#include <QEvent>
#include <QFile>
#include <QFileDialog>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QSpinBox>
#include <QToolBar>
#include <QWidget>
#include <QWindow>

#include "app/iviewer.h"
#include "render/integrator.h"
#include "scene/camera.h"
#include "scene/scene.h"

class Gui : public QMainWindow {
public:
  Gui() = delete;

  // samplesPerPixel is forwarded to the Viewer/Renderer pipeline.
  Gui(int width, int height, const Scene &scene, const Camera &camera,
      const Integrator &integrator, int samplesPerPixel,
      QWindow *parent = nullptr);

  ~Gui();

private:
  void createViewer(bool useGpu);
  void resizeViewer();

  QToolBar *m_toolbar = nullptr;
  QLabel *m_durationText = nullptr;
  QPushButton *m_saveButton = nullptr;
  QPushButton *m_renderButton = nullptr;
  QSpinBox *m_widthInput = nullptr;
  QSpinBox *m_heightInput = nullptr;
  QPushButton *m_fitButton = nullptr;
  QComboBox *m_renderMode = nullptr;
  IViewer *m_viewer = nullptr;

  const Scene &m_scene;
  Camera m_camera;
  const Integrator &m_integrator;
  int m_samplesPerPixel = 1;
};
