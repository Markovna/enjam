#ifndef INCLUDE_ENJAM_RENDERER_BACKEND_H_
#define INCLUDE_ENJAM_RENDERER_BACKEND_H_

#include <enjam/defines.h>
#include <enjam/handle_allocator.h>
#include <array>
#include <vector>
#include <cstdint>
#include <functional>
#include <utility>
#include "program.h"

namespace Enjam {

enum class VertexAttributeType : uint8_t {
  FLOAT,
  FLOAT2,
  FLOAT3,
  FLOAT4,
  BYTE,
  BYTE2,
  BYTE3,
  BYTE4,
  UBYTE,
  UBYTE2,
  UBYTE3,
  UBYTE4,
  SHORT,
  SHORT2,
  SHORT3,
  SHORT4,
  USHORT,
  USHORT2,
  USHORT3,
  USHORT4,
  INT,
  INT2,
  INT3,
  INT4,
  UINT,
  UINT2,
  UINT3,
  UINT4
};

enum class BufferTargetBinding : uint8_t {
  VERTEX,
  UNIFORM
};

enum class DescriptorType : uint8_t {
  UNIFORM_BUFFER,
  TEXTURE
};

enum class TextureFormat : uint16_t {
  RGB8
};

struct DescriptorSetBinding {
  uint8_t binding;
  DescriptorType type;
};

struct DescriptorSetData {
  using BindingsArray = std::vector<DescriptorSetBinding>;
  BindingsArray bindings;
};

struct VertexBufferHW {};
struct IndexBufferHW {};
struct ProgramHW {};
struct DescriptorSetHW {};
struct BufferDataHW {};
struct TextureHW {};

using VertexBufferHandle = Handle<VertexBufferHW>;
using IndexBufferHandle = Handle<IndexBufferHW>;
using ProgramHandle = Handle<ProgramHW>;
using DescriptorSetHandle = Handle<DescriptorSetHW>;
using BufferDataHandle = Handle<BufferDataHW>;
using TextureHandle = Handle<TextureHW>;

struct VertexAttribute {
  static constexpr uint32_t FLAG_NORMALIZED = 0x01;

  VertexAttributeType type = VertexAttributeType::FLOAT;
  uint32_t flags = 0;
  uint32_t offset = 0; // in bytes
  uint8_t stride = 0; // in bytes
};

static constexpr uint32_t VERTEX_ARRAY_MAX_SIZE = 16;

//struct VertexArrayDesc {
//  std::array<VertexAttribute, VERTEX_ARRAY_MAX_SIZE> attributes {};
//  uint8_t attributesCount = 0;
//};

struct BufferDataDesc {
  using Callback = std::function<void(void*, uint64_t)>;

  void* data = nullptr;
  uint64_t size = 0;
  Callback onConsumed = nullptr;

  BufferDataDesc(const BufferDataDesc& other) = delete;
  BufferDataDesc& operator=(const BufferDataDesc& rhs) = delete;

  BufferDataDesc(BufferDataDesc&& other) noexcept
      : data(other.data), size(other.size), onConsumed(std::move(other.onConsumed)) {
    other.data = nullptr;
    other.size = 0;
  }

  BufferDataDesc(void* data, uint64_t size, Callback onConsumed = nullptr)
      : data(data), size(size), onConsumed(std::move(onConsumed)) {}

  BufferDataDesc& operator=(BufferDataDesc&& rhs) noexcept {
    if (this != &rhs) {
      data = rhs.data;
      size = rhs.size;
      onConsumed = rhs.onConsumed;

      rhs.data = nullptr;
      rhs.size = 0;
      rhs.onConsumed = nullptr;
    }
    return *this;
  }
};

class ENJAM_API RendererBackend {
 public:

  virtual ~RendererBackend() = default;
  virtual bool init() = 0;
  virtual void shutdown() = 0;
  virtual void beginFrame() = 0;
  virtual void endFrame() = 0;

  virtual void draw(ProgramHandle,
                    VertexBufferHandle,
                    IndexBufferHandle,
                    uint32_t indexCount = 0,
                    uint32_t indexOffset = 0) = 0;

  virtual ProgramHandle createProgram(ProgramData&) = 0;
  virtual void destroyProgram(ProgramHandle) = 0;

  virtual DescriptorSetHandle createDescriptorSet(DescriptorSetData&&) = 0;
  virtual void destroyDescriptorSet(DescriptorSetHandle) = 0;
  virtual void updateDescriptorSetBuffer(DescriptorSetHandle dsh,
                                         uint8_t binding,
                                         BufferDataHandle bdh,
                                         uint32_t size,
                                         uint32_t offset) = 0;
  virtual void updateDescriptorSetTexture(DescriptorSetHandle dsh, uint8_t binding, TextureHandle th) = 0;
  virtual void bindDescriptorSet(DescriptorSetHandle dsh, uint8_t set) = 0;

  virtual VertexBufferHandle createVertexBuffer(std::initializer_list<VertexAttribute>, uint64_t vertexCount) = 0;
  virtual void assignVertexBufferData(VertexBufferHandle, uint8_t attributeIndex, BufferDataHandle) = 0;
  virtual void destroyVertexBuffer(VertexBufferHandle) = 0;

  virtual IndexBufferHandle createIndexBuffer(uint32_t byteSize) = 0;
  virtual void updateIndexBuffer(IndexBufferHandle, BufferDataDesc&&, uint32_t byteOffset) = 0;
  virtual void destroyIndexBuffer(IndexBufferHandle) = 0;

  virtual BufferDataHandle createBufferData(uint32_t size, BufferTargetBinding) = 0;
  virtual void updateBufferData(BufferDataHandle, BufferDataDesc&&, uint32_t byteOffset) = 0;
  virtual void destroyBufferData(BufferDataHandle) = 0;

  virtual TextureHandle createTexture(uint32_t width, uint32_t height, uint8_t levels, TextureFormat format) = 0;
  virtual void setTextureData(TextureHandle th, uint32_t level, uint32_t xoffset, uint32_t yoffset, uint32_t zoffset,
                              uint32_t width, uint32_t height, uint32_t depth, const void* data) = 0;
  virtual void destroyTexture(TextureHandle) = 0;
};

}

#endif //INCLUDE_ENJAM_RENDERER_BACKEND_H_
