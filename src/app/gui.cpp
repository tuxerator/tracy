#include "app/gui.h"
#include "app/cpu_viewer.h"
#include "app/gpu_viewer.h"

Gui::Gui(int width, int height, const Scene &scene, const Camera &camera,
         const Integrator &integrator, int samplesPerPixel, QWindow *parent)
    : QMainWindow(nullptr),
      m_scene(scene),
      m_camera(camera),
      m_integrator(integrator),
      m_samplesPerPixel(samplesPerPixel) {
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
  m_widthInput->setValue(width);
  toolbar->addWidget(m_widthInput);

  toolbar->addWidget(new QLabel("H:", this));
  m_heightInput = new QSpinBox(this);
  m_heightInput->setRange(1, 10000);
  m_heightInput->setValue(height);
  toolbar->addWidget(m_heightInput);

  m_fitButton = new QPushButton("Fit", this);
  toolbar->addWidget(m_fitButton);

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

  connect(m_fitButton, &QPushButton::clicked, [this]() {
    int fitW = this->width();
    int fitH = this->height() - m_toolbar->height();
    m_widthInput->blockSignals(true);
    m_heightInput->blockSignals(true);
    m_widthInput->setValue(fitW);
    m_heightInput->setValue(fitH);
    m_widthInput->blockSignals(false);
    m_heightInput->blockSignals(false);
    resizeViewer();
  });

  connect(m_renderMode, QOverload<int>::of(&QComboBox::currentIndexChanged),
          [this](int index) {
    createViewer(index == 0);
  });

  resize(width, height);
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
    m_viewer->asWidget()->setFixedSize(w, h);
    m_viewer->render();
}

void Gui::createViewer(bool useGpu) {
  if (m_viewer) {
    m_viewer->stopRaytrace();
    delete m_viewer;
    m_viewer = nullptr;
  }

  int w = m_widthInput->value();
  int h = m_heightInput->value();

  if (useGpu) {
    m_viewer = new GpuViewer(w, h, m_scene, m_camera, m_integrator,
                             m_samplesPerPixel, m_durationText, this);
  } else {
    m_viewer = new CpuViewer(w, h, m_scene, m_camera, m_integrator,
                             m_samplesPerPixel, m_durationText, this);
  }

  m_viewer->asWidget()->setFixedSize(w, h);
  setCentralWidget(m_viewer->asWidget());
  m_viewer->render();
}
