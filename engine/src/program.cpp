#include "enjam/program.h"

namespace Enjam {

ProgramData &ProgramData::setShader(ShaderStage stage, const char* shaderCode) {
  source[(size_t) stage] = std::string(shaderCode);
  return *this;
}

}