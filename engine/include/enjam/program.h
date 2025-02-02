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
  struct Descriptor {
    std::string name;
  };

  static constexpr size_t MAX_DESCRIPTORS_COUNT = 16;
  using ProgramSource = std::array<std::string, (size_t) ShaderStage::COUNT>;
  using DescriptorsMap = std::array<Descriptor, MAX_DESCRIPTORS_COUNT>;

  ProgramData& setShader(ShaderStage, const char *source);
  ProgramSource& getSource() { return source; }

  ProgramData& setDescriptorBinding(const char* name, uint8_t binding) {
    descriptors[binding] = Descriptor { .name = name };
    return *this;
  }

  DescriptorsMap& getDescriptors() { return descriptors; }

 private:
  ProgramSource source;
  DescriptorsMap descriptors;
};

}

#endif //INCLUDE_ENJAM_PROGRAM_H_