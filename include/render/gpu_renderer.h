#pragma once

#include <GL/gl.h>
#include <QOpenGLBuffer>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

class Camera;
class Scene;

class GpuRenderer : protected QOpenGLExtraFunctions {
public:
  GpuRenderer(const Camera &camera);

  ~GpuRenderer();

  bool initialize();
  void beginRender();
  void dispatchSample(int sampleCount);
  void endRender();
  void display(int sampleCount);

  void uploadScene(const Scene &scene);

private:
  void setupDisplayShaders();
  void setupComputeShader();
  void setupGeometry();
  void createOutputTexture(int width, int height);

  const Camera &m_camera;

  QOpenGLShaderProgram *m_displayProgram = nullptr;
  QOpenGLShaderProgram *m_computeProgram = nullptr;
  QOpenGLBuffer m_vbo;
  QOpenGLVertexArrayObject m_vao;

  GLuint m_outputTexture = 0;
  int m_textureWidth = 0;
  int m_textureHeight = 0;

  GLuint m_sphereSSBO = 0;
  GLuint m_triangleSSBO = 0;
  GLuint m_planeSSBO = 0;
  GLuint m_materialSSBO = 0;
  GLuint m_lightSSBO = 0;
  GLuint m_backgroundSSBO = 0;

  int m_numSpheres = 0;
  int m_numTriangles = 0;
  int m_numPlanes = 0;
  int m_numLights = 0;
  int m_numMaterials = 0;
};
