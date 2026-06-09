#pragma once

#include <QComboBox>
#include <QEvent>
#include <QFile>
#include <QFileDialog>
#include <QGraphicsProxyWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QSlider>
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

  Gui(int width, int height, const Scene &scene, const Camera &camera,
      const Integrator &integrator, int samplesPerPixel,
      QWindow *parent = nullptr);

  ~Gui();

private:
  void createViewer(bool useGpu);
  void resizeViewer();
  void applyZoom();
  bool eventFilter(QObject *obj, QEvent *event) override;

  QToolBar *m_toolbar = nullptr;
  QLabel *m_durationText = nullptr;
  QPushButton *m_saveButton = nullptr;
  QPushButton *m_renderButton = nullptr;
  QSpinBox *m_widthInput = nullptr;
  QSpinBox *m_heightInput = nullptr;
  QPushButton *m_fitButton = nullptr;
  QSlider *m_samplesSlider = nullptr;
  QLabel *m_samplesLabel = nullptr;
  QSlider *m_zoomSlider = nullptr;
  QLabel *m_zoomLabel = nullptr;
  QComboBox *m_renderMode = nullptr;
  IViewer *m_viewer = nullptr;

  QGraphicsScene *m_graphicsScene = nullptr;
  QGraphicsView *m_graphicsView = nullptr;
  QGraphicsProxyWidget *m_graphicsProxy = nullptr;

  const Scene &m_scene;
  Camera m_camera;
  const Integrator &m_integrator;
  int m_samplesPerPixel = 1;
};
