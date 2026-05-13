#include "render/gpu_renderer.h"

#include <GL/gl.h>
#include <QFile>
#include <QVector3D>
#include <map>

#include "geometry/plane.h"
#include "geometry/sphere.h"
#include "geometry/triangle.h"
#include "render/shader.h"
#include "scene/camera.h"
#include "scene/scene.h"
#include "shading/light.h"
#include "shading/material.h"

GpuRenderer::~GpuRenderer() {
  delete m_displayProgram;
  delete m_computeProgram;
  m_vbo.destroy();
  m_vao.destroy();
  if (m_outputTexture) {
    glDeleteTextures(1, &m_outputTexture);
  }
  auto deleteSSBO = [this](GLuint &ssbo) {
    if (ssbo) {
      glDeleteBuffers(1, &ssbo);
      ssbo = 0;
    }
  };
  deleteSSBO(m_sphereSSBO);
  deleteSSBO(m_triangleSSBO);
  deleteSSBO(m_planeSSBO);
  deleteSSBO(m_materialSSBO);
  deleteSSBO(m_lightSSBO);
  deleteSSBO(m_backgroundSSBO);
}

bool GpuRenderer::initialize() {
  initializeOpenGLFunctions();
  setupDisplayShaders();
  setupComputeShader();
  setupGeometry();
  return true;
}

void GpuRenderer::render(const Camera &camera) {
  int w = camera.imageWidth();
  int h = camera.imageHeight();

  if (w != m_textureWidth || h != m_textureHeight) {
    createOutputTexture(w, h);
  }

  // --- Compute pass ---
  m_computeProgram->bind();

  m_computeProgram->setUniformValue(
      "u_cameraPos",
      QVector3D(camera.origin().x, camera.origin().y, camera.origin().z));
  m_computeProgram->setUniformValue(
      "u_cameraDir", QVector3D(camera.direction().x, camera.direction().y,
                               camera.direction().z));
  m_computeProgram->setUniformValue(
      "u_cameraRight",
      QVector3D(camera.right().x, camera.right().y, camera.right().z));
  m_computeProgram->setUniformValue(
      "u_cameraUp", QVector3D(camera.up().x, camera.up().y, camera.up().z));
  m_computeProgram->setUniformValue("u_halfWidth",
                                    static_cast<float>(camera.halfWidth()));
  m_computeProgram->setUniformValue("u_halfHeight",
                                    static_cast<float>(camera.halfHeight()));
  m_computeProgram->setUniformValue("u_imageWidth", w);
  m_computeProgram->setUniformValue("u_imageHeight", h);

  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_sphereSSBO);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_triangleSSBO);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_planeSSBO);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_materialSSBO);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, m_lightSSBO);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, m_backgroundSSBO);

  m_computeProgram->setUniformValue("u_numSpheres", m_numSpheres);
  m_computeProgram->setUniformValue("u_numTriangles", m_numTriangles);
  m_computeProgram->setUniformValue("u_numPlanes", m_numPlanes);
  m_computeProgram->setUniformValue("u_numLights", m_numLights);

  glBindImageTexture(0, m_outputTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY,
                     GL_RGBA32F);
  glDispatchCompute(static_cast<GLuint>((w + 15) / 16),
                    static_cast<GLuint>((h + 15) / 16), 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

  m_computeProgram->release();

  // --- Display pass ---
  glViewport(0, 0, w, h);

  m_displayProgram->bind();

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_outputTexture);
  m_displayProgram->setUniformValue("u_renderOutput", 0);

  QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glFinish();

  m_displayProgram->release();
}

void GpuRenderer::setupDisplayShaders() {
  m_displayProgram = new QOpenGLShaderProgram();

  QString vpath =
      QString("%1/src/render/shader/display.vert").arg(PROJECT_SOURCE_DIR);
  QString fpath =
      QString("%1/src/render/shader/display.frag").arg(PROJECT_SOURCE_DIR);

  QString vertSource = Shader::preprocess(vpath);
  QString fragSource = Shader::preprocess(fpath);

  if (!vertSource.isEmpty() && !m_displayProgram->addShaderFromSourceCode(
                                   QOpenGLShader::Vertex, vertSource)) {
    qWarning() << "Display vertex shader error:" << m_displayProgram->log();
  }
  if (!fragSource.isEmpty() && !m_displayProgram->addShaderFromSourceCode(
                                   QOpenGLShader::Fragment, fragSource)) {
    qWarning() << "Display fragment shader error:" << m_displayProgram->log();
  }
  if (!m_displayProgram->link()) {
    qWarning() << "Display shader link error:" << m_displayProgram->log();
  }
}

void GpuRenderer::setupComputeShader() {
  m_computeProgram = new QOpenGLShaderProgram();

  QString cpath =
      QString("%1/src/render/shader/raytrace.comp").arg(PROJECT_SOURCE_DIR);
  QString compSource = Shader::preprocess(cpath);

  if (!compSource.isEmpty() && !m_computeProgram->addShaderFromSourceCode(
                                   QOpenGLShader::Compute, compSource)) {
    qWarning() << "Compute shader error:" << m_computeProgram->log();
  }
  if (!m_computeProgram->link()) {
    qWarning() << "Compute shader link error:" << m_computeProgram->log();
  }
}

void GpuRenderer::setupGeometry() {
  float vertices[] = {-1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f};

  m_vao.create();
  QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

  m_vbo.create();
  m_vbo.bind();
  m_vbo.allocate(vertices, sizeof(vertices));

  m_displayProgram->enableAttributeArray(0);
  m_displayProgram->setAttributeBuffer(0, GL_FLOAT, 0, 2);
}

void GpuRenderer::createOutputTexture(int width, int height) {
  if (m_outputTexture) {
    glDeleteTextures(1, &m_outputTexture);
  }

  glGenTextures(1, &m_outputTexture);
  glBindTexture(GL_TEXTURE_2D, m_outputTexture);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, width, height);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  m_textureWidth = width;
  m_textureHeight = height;
}

void GpuRenderer::uploadScene(const Scene &scene) {
  std::vector<GPUSphere> spheres;
  std::vector<GPUTriangle> triangles;
  std::vector<GPUPlane> planes;
  std::vector<GPUMaterial> materials;
  std::vector<GPUPointLight> lights;

  std::map<std::shared_ptr<Material>, int> matMap;

  auto getMat = [&](const std::shared_ptr<Material> &m) -> int {
    auto it = matMap.find(m);

    if (it != matMap.end()) {
      return it->second;
    }

    int idx = static_cast<int>(materials.size());
    materials.push_back(m->toGPU());
    matMap[m] = idx;
    return idx;
  };

  for (const auto &prim : scene.primitives()) {
    int matIdx = getMat(prim->material());
    if (auto s = std::dynamic_pointer_cast<const Sphere>(prim)) {
      spheres.push_back(s->toGPU(matIdx));
    } else if (auto t = std::dynamic_pointer_cast<const Triangle>(prim)) {
      triangles.push_back(t->toGPU(matIdx));
    } else if (auto p = std::dynamic_pointer_cast<const Plane>(prim)) {
      planes.push_back(p->toGPU(matIdx));
    }
  }

  for (const auto &light : scene.lights()) {
    if (auto pl = std::dynamic_pointer_cast<const PointLight>(light)) {
      lights.push_back(pl->toGPU());
    }
  }

  auto uploadSSBO = [this](GLuint &ssbo, const void *data, size_t size) {
    if (ssbo) {
      glDeleteBuffers(1, &ssbo);
    }

    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_STATIC_DRAW);
  };

  uploadSSBO(m_sphereSSBO, spheres.data(), spheres.size() * sizeof(GPUSphere));
  uploadSSBO(m_triangleSSBO, triangles.data(),
             triangles.size() * sizeof(GPUTriangle));
  uploadSSBO(m_planeSSBO, planes.data(), planes.size() * sizeof(GPUPlane));
  uploadSSBO(m_materialSSBO, materials.data(),
             materials.size() * sizeof(GPUMaterial));
  uploadSSBO(m_lightSSBO, lights.data(), lights.size() * sizeof(GPUPointLight));

  glm::vec3 background = scene.background();
  uploadSSBO(m_backgroundSSBO, &background, sizeof(background));

  m_numSpheres = static_cast<int>(spheres.size());
  m_numTriangles = static_cast<int>(triangles.size());
  m_numPlanes = static_cast<int>(planes.size());
  m_numMaterials = static_cast<int>(materials.size());
  m_numLights = static_cast<int>(lights.size());
}
