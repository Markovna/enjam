#include <enjam/renderer_backend_opengl.h>
#include <enjam/log.h>
#include <enjam/assert.h>
#include <enjam/type_traits_helpers.h>

#include <utility>

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
  , swapChain(std::move(swapChain))
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
  bool loaded = loadGLLoaderIfNeeded(loaderProc);
  if (!loaded) {
    ENJAM_ERROR("Failed to load OpenGL functions");
    return false;
  }

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

uint32_t compileShader(GLenum stage, const char* str) {
  uint32_t id;
  id = glCreateShader(stage);
  glShaderSource(id, 1, &str, NULL);
  glCompileShader(id);

  // check for compile errors
  int32_t success;
  glGetShaderiv(id, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(id, 512, NULL, infoLog);
    ENJAM_ERROR("GLRendererAPI shader compile error #{}", infoLog);
  }
  return id;
}

ProgramHandle RendererBackendOpengl::createProgram(ProgramData& data) {
  auto ph = handleAllocator.allocAndConstruct<GLProgram>();
  auto p = handleAllocator.cast<GLProgram*>(ph);

  uint32_t id = glCreateProgram();

  auto& source = data.getSource();
  uint32_t vertex = compileShader(GL_VERTEX_SHADER, source[(size_t)ShaderStage::VERTEX].c_str());
  uint32_t fragment = compileShader(GL_FRAGMENT_SHADER, source[(size_t)ShaderStage::FRAGMENT].c_str());

  glAttachShader(id, vertex);
  glAttachShader(id, fragment);
  glLinkProgram(id);

  int32_t success;
  glGetProgramiv(id, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetProgramInfoLog(id, 512, NULL, infoLog);
    ENJAM_ERROR("GLRendererAPI shader compile error #{}", infoLog);
  }

  glDeleteShader(vertex);
  glDeleteShader(fragment);

  GL_CHECK_ERRORS();

  ProgramData::DescriptorsMap& descriptors = data.getDescriptors();
  for(auto binding = 0; binding < descriptors.size(); ++binding) {
    auto& desc = descriptors[binding];
    if(desc.name.empty()) { continue; }

    uint32_t index = glGetUniformBlockIndex(id, desc.name.c_str());
    glUniformBlockBinding(id, index, binding);
  }
  GL_CHECK_ERRORS();

  p->id = id;
  return ph;
}

void RendererBackendOpengl::destroyProgram(ProgramHandle ph) {
  auto p = handleAllocator.cast<GLProgram*>(ph);
  glDeleteProgram(p->id);
  GL_CHECK_ERRORS();

  handleAllocator.dealloc(ph, p);
}

DescriptorSetHandle RendererBackendOpengl::createDescriptorSet(DescriptorSetData&& data) {
  auto dsh = handleAllocator.allocAndConstruct<GLDescriptorSet>(std::move(data));
  return dsh;
}

void RendererBackendOpengl::destroyDescriptorSet(DescriptorSetHandle dsh) {
  auto ds = handleAllocator.cast<GLDescriptorSet*>(dsh);

  handleAllocator.dealloc(dsh, ds);
}

void RendererBackendOpengl::updateDescriptorSetBuffer(DescriptorSetHandle dsh, uint8_t binding, BufferDataHandle bdh, uint32_t size, uint32_t offset) {
  auto ds = handleAllocator.cast<GLDescriptorSet*>(dsh);
  auto bd = handleAllocator.cast<GLBufferData*>(bdh);

  ENJAM_ASSERT(bd->target == GL_UNIFORM_BUFFER);

  auto& buffer = std::get<GLDescriptorBuffer>(ds->descriptors[binding]);
  buffer.id = bd->id;
  buffer.size = size;
  buffer.offset = offset;
}

void RendererBackendOpengl::bindDescriptorSet(DescriptorSetHandle dsh) {
  boundDescriptorSetHandle = dsh;
}

void GLDescriptorBuffer::bind(uint8_t binding) const {
  glBindBufferRange(GL_UNIFORM_BUFFER, binding, id, offset, size);
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
  ENJAM_ASSERT(bd->target == GL_ARRAY_BUFFER);
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

  ib->size = size;

  return ibh;
}

BufferDataHandle RendererBackendOpengl::createBufferData(uint32_t size, BufferTargetBinding bufferBinding) {
  auto bdh = handleAllocator.allocAndConstruct<GLBufferData>();
  auto bd = handleAllocator.cast<GLBufferData*>(bdh);

  auto target = OpenGL::toBufferBinding(bufferBinding);
  glGenBuffers(1, &bd->id);
  glBindBuffer(target, bd->id);
  glBufferData(target, size, nullptr, GL_STATIC_DRAW);
  GL_CHECK_ERRORS();

  bd->size = size;
  bd->target = target;
  return bdh;
}

void RendererBackendOpengl::destroyIndexBuffer(IndexBufferHandle ibh) {
  auto ib = handleAllocator.cast<GLIndexBuffer*>(ibh);
  glDeleteBuffers(1, &ib->id);
  GL_CHECK_ERRORS();

  handleAllocator.dealloc(ibh, ib);
}

void RendererBackendOpengl::updateIndexBuffer(IndexBufferHandle ibh, BufferDataDesc&& dataDesc, uint32_t byteOffset) {
  auto ib = handleAllocator.cast<GLIndexBuffer*>(ibh);
  auto binding = GL_ELEMENT_ARRAY_BUFFER;
  glBindBuffer(binding, ib->id);
  glBufferSubData(binding, byteOffset, dataDesc.size, dataDesc.data);
  GL_CHECK_ERRORS();

  if(dataDesc.onConsumed) {
    dataDesc.onConsumed(dataDesc.data, dataDesc.size);
  }
}

void RendererBackendOpengl::updateBufferData(BufferDataHandle bdh, BufferDataDesc&& dataDesc, uint32_t byteOffset) {
  auto bd = handleAllocator.cast<GLBufferData*>(bdh);

  ENJAM_ASSERT(byteOffset + bd->size <= bd->size)

  auto target = bd->target;
  glBindBuffer(target, bd->id);
  glBufferSubData(target, byteOffset, dataDesc.size, dataDesc.data);
  GL_CHECK_ERRORS();

  if(dataDesc.onConsumed) {
    dataDesc.onConsumed(dataDesc.data, dataDesc.size);
  }
}

void RendererBackendOpengl::destroyBufferData(BufferDataHandle bdh) {
  auto bd = handleAllocator.cast<GLBufferData*>(bdh);
  glDeleteBuffers(1, &bd->id);
  GL_CHECK_ERRORS();

  handleAllocator.dealloc(bdh, bd);
}

void RendererBackendOpengl::updateVertexArray(const VertexArrayDesc& vertexArray) {
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

void RendererBackendOpengl::draw(ProgramHandle ph, VertexBufferHandle vbh, IndexBufferHandle ibh, uint32_t indexCount, uint32_t indexOffset) {
  auto program = handleAllocator.cast<GLProgram*>(ph);
  auto vb = handleAllocator.cast<GLVertexBuffer*>(vbh);
  auto ib = handleAllocator.cast<GLIndexBuffer*>(ibh);

  if(boundDescriptorSetHandle) {
    auto ds = handleAllocator.cast<GLDescriptorSet*>(boundDescriptorSetHandle);
    auto& descriptors = ds->descriptors;
    for (auto binding = 0; binding < descriptors.size(); ++binding) {
      auto& d = descriptors[binding];

      std::visit(overloaded {
          [](GLDescriptorNone& arg) { },
          [&binding](GLDescriptorBuffer& arg) { arg.bind(binding); },
          [](GLDescriptorTexture& arg) { ENJAM_ASSERT("Binding texture is not implemented yet."); }
      }, d);
    }
  }

  glUseProgram(program->id);

  glBindVertexArray(defaultVertexArray);

  glBindBuffer(GL_ARRAY_BUFFER, vb->bufferId);
  updateVertexArray(vb->vertexArray);

  if(indexCount == 0) {
    indexCount = ib->size;
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->id);
  void* pointer = (void*) (uintptr_t) (OpenGL::indexOffsetToByteOffset(indexOffset));
  glDrawElements(GL_TRIANGLES, (GLsizei) indexCount, GL_UNSIGNED_INT, pointer);
  GL_CHECK_ERRORS();
}

}