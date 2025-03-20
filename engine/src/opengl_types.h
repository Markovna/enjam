#ifndef ENJAM_ENGINE_SRC_OPENGL_TYPES_H_
#define ENJAM_ENGINE_SRC_OPENGL_TYPES_H_

#include <enjam/renderer_backend.h>
#include <glad/glad.h>

namespace Enjam::OpenGL {

constexpr inline GLboolean toGLBoolean(bool value) {
  return value ? GL_TRUE : GL_FALSE;
}

constexpr inline GLenum toBufferBinding(BufferTargetBinding type) {
  switch (type) {
    case BufferTargetBinding::UNIFORM: return GL_UNIFORM_BUFFER;
    case BufferTargetBinding::VERTEX: return GL_ARRAY_BUFFER;
  }
}

constexpr inline GLint toGLVertexAttribSize(VertexAttributeType type) {
  using Type = VertexAttributeType;
  switch (type) {
    case Type::FLOAT:
    case Type::BYTE:
    case Type::UBYTE:
    case Type::SHORT:
    case Type::USHORT:
    case Type::INT:
    case Type::UINT:
      return 1;

    case Type::FLOAT2:
    case Type::BYTE2:
    case Type::UBYTE2:
    case Type::SHORT2:
    case Type::USHORT2:
    case Type::INT2:
    case Type::UINT2:
      return 2;

    case Type::FLOAT3:
    case Type::BYTE3:
    case Type::UBYTE3:
    case Type::SHORT3:
    case Type::USHORT3:
    case Type::INT3:
    case Type::UINT3:
      return 3;

    case Type::FLOAT4:
    case Type::BYTE4:
    case Type::UBYTE4:
    case Type::SHORT4:
    case Type::USHORT4:
    case Type::INT4:
    case Type::UINT4:
      return 4;
  }

  ENJAM_ASSERT(false && "Unknown vertex attribute type");
  return GL_NONE;
}

constexpr inline uint32_t indexOffsetToByteOffset(uint32_t indexOffset) {
  // Size in bytes of indices in the index buffer
  // Assuming that index buffer contains uint32 values
  uint8_t const indicesShift = 2;
  return indexOffset << indicesShift;
}

constexpr inline GLenum toGLTextureInternalFormat(TextureFormat format) noexcept {
  // TODO
  return GL_RGB8;
}

constexpr inline GLenum toGLPixelFormat(GLenum internalFormat) noexcept {
  // TODO
  return GL_RGB;
}

constexpr inline GLenum toGLPixelType(GLenum internalFormat) noexcept {
  // TODO
  return GL_UNSIGNED_BYTE;
}

constexpr inline GLenum toGLVertexAttribType(VertexAttributeType type) {
  using Type = VertexAttributeType;
  switch (type) {
    case Type::FLOAT:
    case Type::FLOAT2:
    case Type::FLOAT3:
    case Type::FLOAT4:
      return GL_FLOAT;

    case Type::BYTE:
    case Type::BYTE2:
    case Type::BYTE3:
    case Type::BYTE4:
      return GL_BYTE;

    case Type::UBYTE:
    case Type::UBYTE2:
    case Type::UBYTE3:
    case Type::UBYTE4:
      return GL_UNSIGNED_BYTE;

    case Type::SHORT:
    case Type::SHORT2:
    case Type::SHORT3:
    case Type::SHORT4:
      return GL_SHORT;

    case Type::USHORT:
    case Type::USHORT2:
    case Type::USHORT3:
    case Type::USHORT4:
      return GL_UNSIGNED_SHORT;

    case Type::INT:
    case Type::INT2:
    case Type::INT3:
    case Type::INT4:
      return GL_INT;

    case Type::UINT:
    case Type::UINT2:
    case Type::UINT3:
    case Type::UINT4:
      return GL_UNSIGNED_INT;
  }

  ENJAM_ASSERT(false && "Unknown vertex attribute type");
  return GL_NONE;
}

}

#endif //ENJAM_ENGINE_SRC_OPENGL_TYPES_H_
