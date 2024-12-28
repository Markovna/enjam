#include <enjam/renderer_backend_opengl.h>
#include <enjam/log.h>
#include <enjam/assert.h>

#include "opengl_types.h"

namespace Enjam {

#define GL_API_DEBUG 1

#if GL_API_DEBUG
#	define GL_ERRORS(__func) __func, checkErrors(__ENJAM_LOG_LOCATION)
#	define GL_CHECK_ERRORS() checkErrors(__ENJAM_LOG_LOCATION)
#else
#   define GL_ERRORS(__func) __func
#	define GL_CHECK_ERRORS()
#endif // GL_API_DEBUG

static void checkErrors(const char* location) {
  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    Enjam::Log::error(location, "GLRendererAPI error #{}", err);
  }
}

RendererBackendOpengl::RendererBackendOpengl(GLLoaderProc loaderProc, GLSwapChain swapChain)
  : loaderProc(loaderProc)
  , swapChain(swapChain)
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

static void create_temp_shader(GLuint* program) {
  const char *vertexShaderSource = "#version 410 core\n"
                                   "layout (location = 0) in vec2 aPos;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
                                   "}\0";
  const char *fragmentShaderSource = "#version 410 core\n"
                                     "out vec4 FragColor;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                     "}\n\0";

  // build and compile our shader program
  // ------------------------------------
  // vertex shader
  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  // check for shader compile errors
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
  }
  // fragment shader
  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  // check for shader compile errors
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
  }
  // link shaders
  unsigned int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  // check for linking errors
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  *program = shaderProgram;
}

bool RendererBackendOpengl::init() {
  bool loaded = loadGLLoaderIfNeeded(loaderProc);
  if (!loaded) {
    ENJAM_ERROR("Failed to load OpenGL functions");
    return false;
  }

  GL_CHECK_ERRORS();

  create_temp_shader(&program);
  GL_CHECK_ERRORS();

  ENJAM_ASSERT(program);

  glGenVertexArrays(1, &defaultVertexArray);
  GL_CHECK_ERRORS();

  return true;
}

void RendererBackendOpengl::shutdown() {

}

void RendererBackendOpengl::beginFrame() {
  swapChain.makeCurrent();

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  GL_CHECK_ERRORS();
}

void RendererBackendOpengl::endFrame() {
  swapChain.swapBuffers();
}

VertexBufferHandle RendererBackendOpengl::createVertexBuffer(VertexArrayDesc vertexArrayDesc) {
  auto vbh = handleAllocator.allocAndConstruct<GLVertexBuffer>();
  auto vb = handleAllocator.cast<GLVertexBuffer*>(vbh);

  vb->vertexArray = vertexArrayDesc;
  return vbh;
}

void RendererBackendOpengl::assignVertexBufferData(VertexBufferHandle vbh, BufferDataHandle bdh) {
  auto vb = handleAllocator.cast<GLVertexBuffer*>(vbh);
  auto bd = handleAllocator.cast<GLBufferData*>(bdh);
  vb->bufferId = bd->id;
}

void RendererBackendOpengl::destroyVertexBuffer(VertexBufferHandle vbh) {
  auto vb = handleAllocator.cast<GLVertexBuffer*>(vbh);

  handleAllocator.dealloc(vbh, vb);
}

IndexBufferHandle RendererBackendOpengl::createIndexBuffer(uint32_t size) {
  auto ibh = handleAllocator.allocAndConstruct<GLIndexBuffer>();
  auto ib = handleAllocator.cast<GLIndexBuffer*>(ibh);

  glGenBuffers(1, &ib->id);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
  GL_CHECK_ERRORS();

  return ibh;
}

BufferDataHandle RendererBackendOpengl::createBufferData(uint32_t size) {
  auto bdh = handleAllocator.allocAndConstruct<GLBufferData>();
  auto bd = handleAllocator.cast<GLBufferData*>(bdh);

  glGenBuffers(1, &bd->id);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bd->id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
  GL_CHECK_ERRORS();

  bd->size = size;
  return bdh;
}

void RendererBackendOpengl::destroyIndexBuffer(IndexBufferHandle ibh) {
  auto ib = handleAllocator.cast<GLIndexBuffer*>(ibh);
  glDeleteBuffers(1, &ib->id);
  GL_CHECK_ERRORS();

  handleAllocator.dealloc(ibh, ib);
}

void RendererBackendOpengl::updateIndexBuffer(IndexBufferHandle ibh, BufferDataDesc&& dataDesc, uint32_t offset) {
  auto ib = handleAllocator.cast<GLIndexBuffer*>(ibh);
  auto binding = GL_ELEMENT_ARRAY_BUFFER;
  glBindBuffer(binding, ib->id);
  glBufferSubData(binding, offset, dataDesc.size, dataDesc.data);
  GL_CHECK_ERRORS();

  dataDesc.onConsumed(dataDesc.data, dataDesc.size);
}

void RendererBackendOpengl::updateBufferData(BufferDataHandle bdh, BufferDataDesc&& dataDesc, uint32_t offset) {
  auto bd = handleAllocator.cast<GLBufferData*>(bdh);

  ENJAM_ASSERT(offset + bd->size <= bd->size)

  auto binding = GL_ARRAY_BUFFER;
  glBindBuffer(binding, bd->id);
  glBufferSubData(binding, offset, dataDesc.size, dataDesc.data);
  GL_CHECK_ERRORS();

  dataDesc.onConsumed(dataDesc.data, dataDesc.size);
}

void RendererBackendOpengl::destroyBufferData(BufferDataHandle bdh) {
  auto bd = handleAllocator.cast<GLBufferData*>(bdh);
  glDeleteBuffers(1, &bd->id);
  GL_CHECK_ERRORS();

  handleAllocator.dealloc(bdh, bd);
}

void RendererBackendOpengl::bindVertexArray(const VertexArrayDesc& vertexArray) {
  for(auto i = 0; i < vertexArray.attributes.size(); ++i) {
    const VertexAttribute& attribute = vertexArray.attributes[i];
    bool enabled = attribute.flags & VertexAttribute::FLAG_ENABLED;
    if(!enabled) {
      glDisableVertexAttribArray(i);
      GL_CHECK_ERRORS();
      continue;
    }

    GLint size = OpenGL::toGLVertexAttribSize(attribute.type);
    GLboolean normalized = OpenGL::toGLBoolean(attribute.flags & VertexAttribute::FLAG_NORMALIZED);
    GLenum type = OpenGL::toGLVertexAttribType(attribute.type);
    GLsizei stride = vertexArray.stride;
    void* pointer = (void*) (uintptr_t) (attribute.offset);

    glVertexAttribPointer(i, size, type, normalized, stride, pointer);
    glEnableVertexAttribArray(i);
    GL_CHECK_ERRORS();
  }

}

void RendererBackendOpengl::draw(VertexBufferHandle vbh, IndexBufferHandle ibh, uint32_t count) {
  auto vb = handleAllocator.cast<GLVertexBuffer*>(vbh);
  auto ib = handleAllocator.cast<GLIndexBuffer*>(ibh);

  glUseProgram(program);

  glBindVertexArray(defaultVertexArray);

  glBindBuffer(GL_ARRAY_BUFFER, vb->bufferId);
  bindVertexArray(vb->vertexArray);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->id);
  glDrawElements(GL_TRIANGLES, (GLsizei) count, GL_UNSIGNED_INT, nullptr);
  GL_CHECK_ERRORS();
}

}