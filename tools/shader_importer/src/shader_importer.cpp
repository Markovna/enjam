#include <unordered_set>
#include <filesystem>
#include <sstream>
#include <enjam/assets_repository.h>
#include <glslang/Public/ShaderLang.h>
#include <glslang/Public/ResourceLimits.h>
#include <SPIRV/SpvTools.h>

using Path = std::filesystem::path;

EShLanguage toShadingStage(const std::string& ext) {
  if(ext == ".vert") {
    return EShLangVertex;
  } else if(ext == ".frag") {
    return EShLangFragment;
  }

  return EShLangCount;
}

bool generateAsset(const std::vector<Path>& inputPaths,
                   const Path& outputPath) {
  using namespace Enjam;

  struct Source {
    char* text;
    glslang::TShader* shader;
    EShLanguage stage;
    std::filesystem::path path;

    Source(const char* str, EShLanguage stage, const std::filesystem::path& path)
      : text(strdup(str)), shader(new glslang::TShader(stage)), stage(stage), path(path)
    {
    }

    Source(Source&) = delete;
    Source(Source&& other) noexcept : text(other.text), shader(other.shader), stage(other.stage), path(std::move(other.path)) {
      other.text = nullptr;
      other.shader = nullptr;
    }

    Source& operator=(Source&) = delete;
    Source& operator=(Source&&) = delete;

    ~Source() {
      if(text != nullptr) {
        free(text);
      }

      if(shader != nullptr) {
        delete shader;
      }
    }
  };

  std::vector<Source> sources;
  sources.reserve(inputPaths.size());
  for(auto& path : inputPaths) {
    std::filesystem::path ext = path.extension();
    EShLanguage stage = toShadingStage(ext);
    if (stage == EShLangCount) {
      std::cout << "Unsupported file extension " << ext << "\n";
      continue;
    }

    std::ifstream file(path);
    std::istreambuf_iterator<char> begin(file), end;
    std::string tempString(begin, end);
    sources.emplace_back(tempString.c_str(), stage, path);
  }

  auto includer = glslang::TShader::ForbidIncluder { };

  const int version = 450;
  glslang::TProgram program;
  for(auto& source : sources) {
    glslang::TShader* shader = source.shader;
    shader->setDebugInfo(true);
    shader->setStrings(&source.text, 1);
    shader->setEnhancedMsgs();
    shader->setEnvInput(glslang::EShSourceGlsl, source.stage, glslang::EShClientOpenGL, version);
    shader->setEnvClient(glslang::EShClientOpenGL, glslang::EShTargetOpenGL_450);
//    shader->setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_0);
//    shader.setEnvInputVulkanRulesRelaxed();

    EShMessages messages = EShMsgDefault;
    bool result = shader->parse(GetResources(), version, false, messages, includer);

    if(strlen(shader->getInfoLog()) > 0) {
      ENJAM_INFO("Info: ", shader->getInfoLog());
    }
    if(!result) {
      ENJAM_ERROR("Couldn't parse shader {}", source.path.string());
      ENJAM_ERROR("Shader code:\n{}", source.text);

      if(strlen(shader->getInfoDebugLog()) > 0) {
        ENJAM_ERROR("Debug: ", shader->getInfoDebugLog());
      }
      continue;
    }

    program.addShader(shader);
  }

  ENJAM_INFO("Start linking...");

  EShMessages messages = EShMsgDefault;
  if(!program.link(messages)) {
    ENJAM_ERROR("Link failed! {} {}", program.getInfoLog(), program.getInfoDebugLog());
    return false;
  } else {
    ENJAM_INFO("Link succeeded! {} {}", program.getInfoLog(), program.getInfoDebugLog());
  }

  std::vector<std::string> outputFiles;

  using SpirvBlob = std::vector<uint32_t>;
  for(auto& source : sources) {
    glslang::SpvOptions options;
    spv::SpvBuildLogger logger;
    options.generateDebugInfo = true;
    SpirvBlob spirvOutput;
    glslang::GlslangToSpv(*program.getIntermediate((EShLanguage) source.stage), spirvOutput, &logger, &options);

    ENJAM_INFO("{}", logger.getAllMessages().c_str());
  }

  return false;
}

int main(int argc, char* argv[]) {
  std::filesystem::path output;
  std::vector<Path> inputs;

  std::vector<std::string_view> args {argv + 1, argv + argc};

  auto it = args.begin();
  while(it != args.end()) {
    auto& arg = *it;
    if(arg.empty()) {
      it++;
      continue;
    }

    if(arg[0] == '-') {
      if(arg == "-o") {
        it++;
        output = *it;
      } else {
        throw std::runtime_error("Unknown option: " + std::string(*it));
      }
      it++;
      continue;
    }

    inputs.emplace_back(arg);
    it++;
  }

  if(inputs.empty()) {
    throw std::runtime_error("Input file paths are not provided");
  }

  if(output.empty()) {
    output = inputs.back();
    output.replace_extension("nj_sl");
  }

  generateAsset(inputs, output);
}