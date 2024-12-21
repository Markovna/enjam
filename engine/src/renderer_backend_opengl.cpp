#include <enjam/renderer_backend_opengl.h>
#include <enjam/log.h>
#include <enjam/assert.h>
#include <enjam/platform.h>
#include "glad/glad.h"

namespace Enjam {

#define GL_API_DEBUG 1

#if GL_API_DEBUG
#	define GL_ERRORS(__func) _CHECK_GL_ERRORS(__func)
#else
#   define GL_ERRORS(__func) __func
#endif // GL_API_DEBUG

#define _CHECK_GL_ERRORS(__func) \
    __func, checkErrors()

static void checkErrors() {
  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    ENJAM_ERROR("GLRendererAPI error #{}", err);
  }
}

RendererBackendOpengl::RendererBackendOpengl(Platform& platform)
  : platform(platform)
  , vertexArrays(0)
  { }

static bool loadGLLoaderIfNeeded(GLLoaderProc glLoaderProc) {
  static bool glLoaded = false;
  if(glLoaded) {
    return true;
  }

  glLoaded = gladLoadGLLoader((GLADloadproc) glLoaderProc);
  if(!glLoaded) {
    ENJAM_ERROR("Failed to load OpenGL functions");
  }

  return glLoaded;
}

bool RendererBackendOpengl::init() {
  bool loaded = loadGLLoaderIfNeeded(platform.GetGLLoaderProc());
  if (!loaded) {
    ENJAM_ERROR("Failed to load OpenGL functions");
    return false;
  }

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_CULL_FACE);

  return true;
}

void RendererBackendOpengl::shutdown() {

}

void RendererBackendOpengl::beginFrame() {

}

void RendererBackendOpengl::endFrame() {

}

}