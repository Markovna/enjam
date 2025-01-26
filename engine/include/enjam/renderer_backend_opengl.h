#ifndef INCLUDE_RENDERER_BACKEND_OPENGL_H_
#define INCLUDE_RENDERER_BACKEND_OPENGL_H_

#include <enjam/renderer_backend.h>
#include <enjam/handle_allocator.h>
#include <functional>
#include <type_traits>
#include <glad/glad.h>

namespace Enjam {

typedef void* (*GLLoaderProc)(const char *name);

struct GLSwapChain {
  using MakeCurrentFunc = std::function<void()>;
  using SwapBuffersFunc = std::function<void()>;

  MakeCurrentFunc makeCurrent;
  SwapBuffersFunc swapBuffers;
};

struct GLProgram : public Program {
  GLuint id = 0;
};

struct GLVertexBuffer : public VertexBuffer {
  GLuint bufferId = 0;
  VertexArrayDesc vertexArray;
};

struct GLIndexBuffer : public IndexBuffer {
  GLuint id = 0;
  uint32_t size = 0;
};

struct GLBufferData : public BufferData {
  GLuint id = 0;
  uint32_t size = 0;
};

class RendererBackendOpengl : public RendererBackend {
 public:
  using HandleAllocator = HandleAllocator<GLVertexBuffer, GLIndexBuffer, GLProgram, GLBufferData>;

  explicit RendererBackendOpengl(GLLoaderProc, GLSwapChain);

  bool init() override;
  void shutdown() override;

  void beginFrame() override;
  void endFrame() override;

  void draw(ProgramHandle, VertexBufferHandle, IndexBufferHandle, uint32_t count) override;

  ProgramHandle createProgram(ProgramData&) override;
  void destroyProgram(ProgramHandle) override;

  VertexBufferHandle createVertexBuffer(VertexArrayDesc) override;
  void assignVertexBufferData(VertexBufferHandle, BufferDataHandle) override;
  void destroyVertexBuffer(VertexBufferHandle) override;

  IndexBufferHandle createIndexBuffer(uint32_t size) override;
  void updateIndexBuffer(IndexBufferHandle, BufferDataDesc&&, uint32_t offset) override;
  void destroyIndexBuffer(IndexBufferHandle) override;

  BufferDataHandle createBufferData(uint32_t size) override;
  void updateBufferData(BufferDataHandle, BufferDataDesc&&, uint32_t offset) override;
  void destroyBufferData(BufferDataHandle) override;

 private:
  void updateVertexArray(const VertexArrayDesc&);

 private:
  GLLoaderProc loaderProc;
  GLSwapChain swapChain;
  HandleAllocator handleAllocator;
  GLuint defaultVertexArray;
};

}

#endif //INCLUDE_RENDERER_BACKEND_OPENGL_H_
