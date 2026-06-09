#pragma once

#include <QImage>
#include <QWidget>

class IViewer {
public:
  virtual ~IViewer() = default;

  virtual const QImage &getImage() const = 0;
  virtual void stopRaytrace() = 0;
  virtual void render() = 0;
  virtual QWidget *asWidget() = 0;
  virtual void setSamplesPerPixel(const int samplesPerPixel) = 0;
};
