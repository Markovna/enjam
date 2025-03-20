#ifndef INCLUDE_RENDERER_BACKEND_OPENGL_H_
#define INCLUDE_RENDERER_BACKEND_OPENGL_H_

#include <enjam/renderer_backend.h>
#include <enjam/handle_allocator.h>
#include <bitset>
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

struct GLProgram : public ProgramHW {
  GLuint id = 0;
};

struct GLVertexBuffer : public VertexBufferHW {
  GLuint bufferId = 0;
  VertexArrayDesc vertexArray;
};

struct GLIndexBuffer : public IndexBufferHW {
  GLuint id = 0;
  uint32_t size = 0;
};

struct GLBufferData : public BufferDataHW {
  GLuint id = 0;
  uint32_t size = 0;
  GLenum target = 0; // GL_UNIFORM_BUFFER / GL_ARRAY_BUFFER
};

struct GLTexture : TextureHW {
  GLuint id;
  GLenum target;
  GLenum glFormat;
};

struct GLDescriptorBuffer {
  GLuint id = 0;
  uint32_t size = 0;
  uint32_t offset = 0;

  void bind(uint8_t binding) const;
};

struct GLDescriptorTexture {
  GLuint id;
  GLenum target;

  void bind(uint8_t binding) const;
};

struct GLDescriptorNone {};

using GLDescriptor = std::variant<GLDescriptorNone, GLDescriptorBuffer, GLDescriptorTexture>;

struct GLDescriptorSet : public DescriptorSetHW {
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
  using HandleAllocator = HandleAllocator<GLVertexBuffer, GLIndexBuffer, GLProgram, GLTexture, GLBufferData, GLDescriptorSet>;

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
  void updateDescriptorSetTexture(DescriptorSetHandle dsh, uint8_t binding, TextureHandle th) override;
  void bindDescriptorSet(DescriptorSetHandle dsh, uint8_t set) override;

  VertexBufferHandle createVertexBuffer(VertexArrayDesc) override;
  void assignVertexBufferData(VertexBufferHandle, BufferDataHandle) override;
  void destroyVertexBuffer(VertexBufferHandle) override;

  IndexBufferHandle createIndexBuffer(uint32_t byteSize) override;
  void updateIndexBuffer(IndexBufferHandle, BufferDataDesc&&, uint32_t byteOffset) override;
  void destroyIndexBuffer(IndexBufferHandle) override;

  BufferDataHandle createBufferData(uint32_t size, BufferTargetBinding) override;
  void updateBufferData(BufferDataHandle, BufferDataDesc&&, uint32_t byteOffset) override;
  void destroyBufferData(BufferDataHandle) override;

  TextureHandle createTexture(uint32_t width, uint32_t height, uint8_t levels, TextureFormat format) override;
  void setTextureData(TextureHandle th, uint32_t level, uint32_t xoffset, uint32_t yoffset, uint32_t zoffset,
                      uint32_t width, uint32_t height, uint32_t depth, void* data) override;
  void destroyTexture(TextureHandle) override;

 private:
  using DescriptorSetBitset = std::bitset<DESCRIPTOR_SET_COUNT>;

  void updateVertexArray(const VertexArrayDesc&);
  void updateDescriptorSets(const DescriptorSetBitset&);
  void updateProgramUniformBindings(uint32_t id, const ProgramData::DescriptorsArray& descriptors);
  void updateProgramTextureBindings(uint32_t id, const ProgramData::DescriptorsArray& descriptors);

 private:
  GLLoaderProc loaderProc;
  GLSwapChain swapChain;
  HandleAllocator handleAllocator;
  GLuint defaultVertexArray;
  std::array<DescriptorSetHandle, DESCRIPTOR_SET_COUNT> boundDescriptorSets;
};

}

#endif //INCLUDE_RENDERER_BACKEND_OPENGL_H_
