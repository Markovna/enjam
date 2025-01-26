#ifndef INCLUDE_ENJAM_PROGRAM_H_
#define INCLUDE_ENJAM_PROGRAM_H_

#include <array>
#include <string>

namespace Enjam {

enum class ShaderStage {
  VERTEX = 0,
  FRAGMENT,

  COUNT
};

class ProgramData {
 public:
  using ProgramSource = std::array<std::string, (size_t) ShaderStage::COUNT>;

  ProgramData& setShader(ShaderStage, const char *source);
  ProgramSource& getSource() { return source; }

 private:
  ProgramSource source;
};

}

#endif //INCLUDE_ENJAM_PROGRAM_H_