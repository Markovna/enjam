#ifndef INCLUDE_RENDERER_BACKEND_OPENGL_H_
#define INCLUDE_RENDERER_BACKEND_OPENGL_H_

#include <enjam/renderer_backend.h>
#include <enjam/handle_allocator.h>
#include <functional>
#include <type_traits>
#include <glad/glad.h>

namespace Enjam::renderer {

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
  GLenum target = 0; // GL_UNIFORM_BUFFER / GL_ARRAY_BUFFER
};

struct GLDescriptorBuffer {
  GLuint id = 0;
  uint32_t size = 0;
  uint32_t offset = 0;

  void bind(uint8_t binding) const;
};

struct GLDescriptorTexture {
  // TODO
};

struct GLDescriptorNone {};

using GLDescriptor = std::variant<GLDescriptorNone, GLDescriptorBuffer, GLDescriptorTexture>;

struct GLDescriptorSet : public DescriptorSet {
  explicit GLDescriptorSet(DescriptorSetData &&data) noexcept {
    std::sort(data.bindings.begin(), data.bindings.end(), [](auto &&lhs, auto &&rhs) {
      return lhs.binding < rhs.binding;
    });

    size_t descriptorsCount = data.bindings.back().binding + 1;
    descriptors.resize(descriptorsCount, GLDescriptorNone{});

    for (auto &desc: data.bindings) {
      switch (desc.type) {
        case DescriptorType::UNIFORM_BUFFER: {
          descriptors[desc.binding] = GLDescriptorBuffer{};
          break;
        }
        case DescriptorType::TEXTURE: {
          descriptors[desc.binding] = GLDescriptorTexture{};
          break;
        }
      }
    }
  }

  using DescriptorsArray = std::vector<GLDescriptor>;
  DescriptorsArray descriptors;
};

class RendererBackendOpengl : public RendererBackend {
 public:
  using HandleAllocator = HandleAllocator<GLVertexBuffer, GLIndexBuffer, GLProgram, GLBufferData, GLDescriptorSet>;

  explicit RendererBackendOpengl(GLLoaderProc, GLSwapChain);

  bool init() override;
  void shutdown() override;

  void beginFrame() override;
  void endFrame() override;

  void draw(ProgramHandle, VertexBufferHandle, IndexBufferHandle, uint32_t indexCount, uint32_t indexOffset) override;

  ProgramHandle createProgram(ProgramData&) override;
  void destroyProgram(ProgramHandle) override;

  DescriptorSetHandle createDescriptorSet(DescriptorSetData&&) override;
  void destroyDescriptorSet(DescriptorSetHandle) override;
  void updateDescriptorSetBuffer(DescriptorSetHandle dsh, uint8_t binding, BufferDataHandle bdh, uint32_t size, uint32_t offset) override;
  void bindDescriptorSet(DescriptorSetHandle dsh) override;

  VertexBufferHandle createVertexBuffer(VertexArrayDesc) override;
  void assignVertexBufferData(VertexBufferHandle, BufferDataHandle) override;
  void destroyVertexBuffer(VertexBufferHandle) override;

  IndexBufferHandle createIndexBuffer(uint32_t byteSize) override;
  void updateIndexBuffer(IndexBufferHandle, BufferDataDesc&&, uint32_t byteOffset) override;
  void destroyIndexBuffer(IndexBufferHandle) override;

  BufferDataHandle createBufferData(uint32_t size, BufferTargetBinding) override;
  void updateBufferData(BufferDataHandle, BufferDataDesc&&, uint32_t byteOffset) override;
  void destroyBufferData(BufferDataHandle) override;

 private:
  void updateVertexArray(const VertexArrayDesc&);

 private:
  GLLoaderProc loaderProc;
  GLSwapChain swapChain;
  HandleAllocator handleAllocator;
  GLuint defaultVertexArray;
  DescriptorSetHandle boundDescriptorSetHandle;
};

}

#endif //INCLUDE_RENDERER_BACKEND_OPENGL_H_
