#include "app/gui.h"

#include <QPinchGesture>
#include <QWheelEvent>
#include <qcoreevent.h>
#include <qgraphicsview.h>
#include <qsizepolicy.h>

#include "app/cpu_viewer.h"
#include "app/gpu_viewer.h"

Gui::Gui(int width, int height, const Scene &scene, const Camera &camera,
         const Integrator &integrator, int samplesPerPixel, QWindow *parent)
    : QMainWindow(nullptr), m_scene(scene), m_camera(camera),
      m_integrator(integrator), m_samplesPerPixel(samplesPerPixel) {
  Q_UNUSED(parent);

  QToolBar *toolbar = new QToolBar(this);
  toolbar->setMovable(false);
  m_toolbar = toolbar;

  m_renderMode = new QComboBox(this);
  m_renderMode->addItem("GPU");
  m_renderMode->addItem("CPU");
  toolbar->addWidget(m_renderMode);

  m_saveButton = new QPushButton("Save as ...", this);
  toolbar->addWidget(m_saveButton);

  m_renderButton = new QPushButton("Render", this);
  toolbar->addWidget(m_renderButton);

  toolbar->addWidget(new QLabel("W:", this));
  m_widthInput = new QSpinBox(this);
  m_widthInput->setRange(1, 10000);
  m_widthInput->setValue(this->width());
  toolbar->addWidget(m_widthInput);

  toolbar->addWidget(new QLabel("H:", this));
  m_heightInput = new QSpinBox(this);
  m_heightInput->setRange(1, 10000);
  m_heightInput->setValue(this->height());
  toolbar->addWidget(m_heightInput);

  m_fitButton = new QPushButton("Fit", this);
  toolbar->addWidget(m_fitButton);

  toolbar->addWidget(new QLabel("S:", this));
  m_samplesSlider = new QSlider(Qt::Horizontal, this);
  m_samplesSlider->setRange(1, 10000);
  m_samplesSlider->setValue(m_samplesPerPixel);
  m_samplesSlider->setFixedWidth(100);
  toolbar->addWidget(m_samplesSlider);
  m_samplesLabel = new QLabel(QString::number(m_samplesPerPixel), this);
  m_samplesLabel->setFixedWidth(36);
  toolbar->addWidget(m_samplesLabel);

  toolbar->addWidget(new QLabel("Z:", this));
  m_zoomSlider = new QSlider(Qt::Horizontal, this);
  m_zoomSlider->setRange(10, 1000);
  m_zoomSlider->setValue(100);
  m_zoomSlider->setSingleStep(25);
  m_zoomSlider->setFixedWidth(100);
  toolbar->addWidget(m_zoomSlider);
  m_zoomLabel = new QLabel("100%", this);
  m_zoomLabel->setFixedWidth(40);
  toolbar->addWidget(m_zoomLabel);

  QWidget *spacer = new QWidget();
  spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  toolbar->addWidget(spacer);

  m_durationText = new QLabel(this);
  m_durationText->setText("Rendering...");
  toolbar->addWidget(m_durationText);

  addToolBar(toolbar);

  createViewer(true);

  connect(m_saveButton, &QPushButton::clicked, [this]() {
    QString filename =
        QFileDialog::getSaveFileName(this, tr("Save Image"), "render.png",
                                     tr("Images (*.png);;All Files (*)"));

    if (filename.isEmpty()) {
      return;
    }

    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)) {
      m_viewer->getImage().save(&file, "PNG");
    }
  });

  connect(m_renderButton, &QPushButton::clicked, [this]() {
    m_durationText->setText("Rendering...");
    m_viewer->render();
  });

  connect(m_widthInput, QOverload<int>::of(&QSpinBox::valueChanged),
          [this](int) { resizeViewer(); });

  connect(m_heightInput, QOverload<int>::of(&QSpinBox::valueChanged),
          [this](int) { resizeViewer(); });

  connect(m_samplesSlider, &QSlider::valueChanged, [this](int val) {
    m_samplesPerPixel = val;
    m_samplesLabel->setText(QString::number(val));
    m_viewer->setSamplesPerPixel(m_samplesPerPixel);
  });
  connect(m_samplesSlider, &QSlider::sliderReleased,
          [this]() { m_viewer->render(); });

  connect(m_zoomSlider, &QSlider::valueChanged, [this](int) { applyZoom(); });

  connect(m_fitButton, &QPushButton::clicked, [this]() {
    int fitW = this->width();
    int fitH = this->height() - m_toolbar->height();
    m_widthInput->blockSignals(true);
    m_heightInput->blockSignals(true);
    m_widthInput->setValue(fitW);
    m_heightInput->setValue(fitH);
    m_widthInput->blockSignals(false);
    m_heightInput->blockSignals(false);
    m_zoomSlider->blockSignals(true);
    m_zoomSlider->setValue(100);
    m_zoomSlider->blockSignals(false);
    applyZoom();
    resizeViewer();
  });

  connect(m_renderMode, QOverload<int>::of(&QComboBox::currentIndexChanged),
          [this](int index) { createViewer(index == 0); });
}

Gui::~Gui() {
  if (m_viewer) {
    m_viewer->stopRaytrace();
  }
}

void Gui::resizeViewer() {
  int w = m_widthInput->value();
  int h = m_heightInput->value();
  m_camera.resize(w, h);
  m_graphicsScene->setSceneRect(0, 0, w, h);
  m_viewer->render();
  qWarning() << "Viewer size: " << m_viewer->asWidget()->width() << "x"
             << m_viewer->asWidget()->height();
  qWarning() << "Proxy size: " << m_graphicsProxy->geometry().width() << "x"
             << m_graphicsProxy->geometry().height();
}

void Gui::createViewer(bool useGpu) {
  if (m_viewer) {
    m_viewer->stopRaytrace();
    if (m_graphicsProxy) {
      m_graphicsScene->removeItem(m_graphicsProxy);
      delete m_graphicsProxy;
      m_graphicsProxy = nullptr;
    }
    m_viewer = nullptr;
  }

  if (!m_graphicsView) {
    m_graphicsScene = new QGraphicsScene(this);
    m_graphicsScene->setBackgroundBrush(QColor(32, 32, 32));
    m_graphicsView = new QGraphicsView(m_graphicsScene);
    m_graphicsView->setRenderHint(QPainter::SmoothPixmapTransform, false);
    m_graphicsView->setAlignment(Qt::AlignCenter);
    m_graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    m_graphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    m_graphicsView->viewport()->installEventFilter(this);
    m_graphicsView->viewport()->grabGesture(Qt::PinchGesture);
    setCentralWidget(m_graphicsView);
  }

  if (useGpu) {
    m_viewer = new GpuViewer(m_scene, m_camera, m_integrator, m_samplesPerPixel,
                             m_durationText);
  } else {
    m_viewer = new CpuViewer(m_scene, m_camera, m_integrator, m_samplesPerPixel,
                             m_durationText);
  }

  m_graphicsProxy = m_graphicsScene->addWidget(m_viewer->asWidget());
  m_viewer->asWidget()->installEventFilter(this);
  applyZoom();
  m_viewer->render();
}

void Gui::applyZoom() {
  double z = m_zoomSlider->value() / 100.0;
  m_zoomLabel->setText(QString("%1%").arg(m_zoomSlider->value()));
  m_graphicsView->resetTransform();
  m_graphicsView->scale(z, z);
}

bool Gui::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() == QEvent::Paint && obj == m_viewer->asWidget()) {
    m_graphicsProxy->update();
  }

  if (event->type() == QEvent::Wheel) {
    QWheelEvent *we = static_cast<QWheelEvent *>(event);
    double factor = we->angleDelta().y() > 0 ? 1.1 : 1.0 / 1.1;
    int newVal = qRound(m_zoomSlider->value() * factor);
    newVal = qBound(10, newVal, 1000);
    m_zoomSlider->setValue(newVal);
    return true;
  }

  if (event->type() == QEvent::Gesture) {
    QGestureEvent *ge = static_cast<QGestureEvent *>(event);
    if (QPinchGesture *pinch =
            static_cast<QPinchGesture *>(ge->gesture(Qt::PinchGesture))) {
      if (pinch->state() == Qt::GestureUpdated) {
        int newVal = qRound(m_zoomSlider->value() * pinch->scaleFactor());
        newVal = qBound(10, newVal, 1000);
        m_zoomSlider->setValue(newVal);
      }
      return true;
    }
  }

  return QMainWindow::eventFilter(obj, event);
}
