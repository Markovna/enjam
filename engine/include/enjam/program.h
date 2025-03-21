#ifndef INCLUDE_ENJAM_PROGRAM_H_
#define INCLUDE_ENJAM_PROGRAM_H_

#include <enjam/assert.h>
#include <array>
#include <string>
#include <vector>

namespace Enjam {

enum class ShaderStage {
  VERTEX = 0,
  FRAGMENT,

  COUNT
};

class ProgramData {
 public:
  enum class DescriptorType : uint16_t {
    UNIFORM = 0,
    SAMPLER
  };

  struct DescriptorInfo {
    std::string name;
    DescriptorType type;
  };

  static constexpr size_t MAX_DESCRIPTOR_BINDINGS_COUNT = 16;
  static constexpr size_t DESCRIPTOR_SET_COUNT = 4;
  using ProgramSource = std::array<std::string, (size_t) ShaderStage::COUNT>;
  using DescriptorSetInfo = std::vector<DescriptorInfo>;
  using DescriptorsMap = std::array<DescriptorSetInfo, (size_t) DESCRIPTOR_SET_COUNT>;

  ProgramData& setShader(ShaderStage, const char *source);
  ProgramSource& getSource() { return source; }

  ProgramData& setDescriptorSet(uint8_t set, DescriptorSetInfo&& setInfo) {
    ENJAM_ASSERT(setInfo.size() < MAX_DESCRIPTOR_BINDINGS_COUNT);
    descriptorSets[(size_t) set] = std::move(setInfo);
    return *this;
  }

  const DescriptorsMap& getDescriptorsMap() const { return descriptorSets; }

 private:
  ProgramSource source;
  DescriptorsMap descriptorSets { };
};

}

#endif //INCLUDE_ENJAM_PROGRAM_H_