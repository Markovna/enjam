#include "enjam/program.h"

namespace Enjam {

ProgramData &ProgramData::setShader(ShaderStage stage, const ByteArray& shaderCode) {
  source[(size_t) stage] = shaderCode;
  return *this;
}

}