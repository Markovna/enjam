#ifndef INCLUDE_ENJAM_RENDERER_BACKEND_H_
#define INCLUDE_ENJAM_RENDERER_BACKEND_H_

#include <enjam/defines.h>
#include <array>
#include <vector>
#include <cstdint>
#include <functional>
#include <utility>
#include "program.h"

namespace Enjam {

template<typename T>
class Handle {
 public:
  using HandleId = uint32_t;
  static constexpr HandleId invalidId = HandleId { UINT32_MAX };

  Handle() noexcept = default;

  Handle(const Handle&) noexcept = default;
  Handle& operator=(const Handle&) noexcept = default;

  Handle(Handle&& rhs) noexcept
    : id(rhs.id) {
    rhs.id = invalidId;
  }

  explicit Handle(HandleId id) noexcept : id(id) { }

  Handle& operator=(Handle&& rhs) noexcept {
    if(this != &rhs) {
      id = rhs.id;
      rhs.id = invalidId;
    }
    return *this;
  }

  bool operator ==(Handle other) const noexcept { return id == other.id; }
  bool operator !=(Handle other) const noexcept { return id != other.id; }

  explicit constexpr operator bool() const noexcept {return id != invalidId; }

  template<typename D, typename = std::enable_if_t<std::is_base_of<T, D>::value> >
  Handle(const Handle<D>& derived) noexcept : Handle(derived.id) { }

  HandleId getId() const noexcept { return id; }

 private:

  template<class U> friend class Handle;

 private:
  HandleId id = invalidId;
};

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

struct DescriptorSetBinding {
  uint8_t binding;
  DescriptorType type;
};

struct DescriptorSetData {
  using BindingsArray = std::vector<DescriptorSetBinding>;
  BindingsArray bindings;
};

struct VertexBuffer {};
struct IndexBuffer {};
struct Program {};
struct DescriptorSet {};
struct BufferData {};

using VertexBufferHandle = Handle<VertexBuffer>;
using IndexBufferHandle = Handle<IndexBuffer>;
using ProgramHandle = Handle<Program>;
using DescriptorSetHandle = Handle<DescriptorSet>;
using BufferDataHandle = Handle<BufferData>;

struct VertexAttribute {
  static constexpr uint32_t FLAG_ENABLED = 0x01;
  static constexpr uint32_t FLAG_NORMALIZED = 0x02;

  VertexAttributeType type = VertexAttributeType::FLOAT;
  uint32_t flags = 0;
  uint32_t offset = 0;
};

struct VertexArrayDesc {
  static constexpr uint32_t ARRAY_SIZE = 16;

  std::array<VertexAttribute, ARRAY_SIZE> attributes {};
  uint8_t stride = 0;
};

struct BufferDataDesc {
  using Callback = std::function<void(void*, uint32_t)>;

  void* data = nullptr;
  uint32_t size = 0;
  Callback onConsumed = nullptr;

  BufferDataDesc(const BufferDataDesc& other) = delete;
  BufferDataDesc& operator =(const BufferDataDesc& rhs) = delete;

  BufferDataDesc(BufferDataDesc&& other) noexcept
    : data(other.data)
    , size(other.size)
    , onConsumed(std::move(other.onConsumed)) {
    other.data = nullptr;
    other.size = 0;
  }

  BufferDataDesc(void* data, uint32_t size, Callback onConsumed = nullptr)
    : data(data), size(size), onConsumed(std::move(onConsumed))
  {}

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

  virtual void draw(ProgramHandle, VertexBufferHandle, IndexBufferHandle, uint32_t count) = 0;

  virtual ProgramHandle createProgram(ProgramData&) = 0;
  virtual void destroyProgram(ProgramHandle) = 0;

  virtual DescriptorSetHandle createDescriptorSet(DescriptorSetData&&) = 0;
  virtual void destroyDescriptorSet(DescriptorSetHandle) = 0;
  virtual void updateDescriptorSetBuffer(DescriptorSetHandle dsh, uint8_t binding, BufferDataHandle bdh, uint32_t size, uint32_t offset) = 0;
  virtual void bindDescriptorSet(DescriptorSetHandle dsh) = 0;

  virtual VertexBufferHandle createVertexBuffer(VertexArrayDesc) = 0;
  virtual void assignVertexBufferData(VertexBufferHandle, BufferDataHandle) = 0;
  virtual void destroyVertexBuffer(VertexBufferHandle) = 0;

  virtual IndexBufferHandle createIndexBuffer(uint32_t size) = 0;
  virtual void updateIndexBuffer(IndexBufferHandle, BufferDataDesc&&, uint32_t offset) = 0;
  virtual void destroyIndexBuffer(IndexBufferHandle) = 0;

  virtual BufferDataHandle createBufferData(uint32_t size, BufferTargetBinding) = 0;
  virtual void updateBufferData(BufferDataHandle, BufferDataDesc&&, uint32_t offset) = 0;
  virtual void destroyBufferData(BufferDataHandle) = 0;
};

}

#endif //INCLUDE_ENJAM_RENDERER_BACKEND_H_
