#include <unordered_set>
#include <filesystem>
#include <sstream>
#include <enjam/assets_repository.h>
#include <glslang/Public/ShaderLang.h>
#include <glslang/Public/ResourceLimits.h>

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

  for(auto& path : inputPaths) {
    std::filesystem::path ext = path.extension();
    EShLanguage stage = toShadingStage(ext);
    if(stage == EShLangCount) {
      std::cout << "Unsupported file extension " << ext << "\n";
      continue;
    }

    std::ifstream file(path);
    std::stringstream stream;
    stream << file.rdbuf();
    std::string str = stream.str();

    const char* text[] = {
        str.c_str()
    };

    glslang::TShader shader { stage };
    shader.setDebugInfo(true);
    shader.setSourceFile(path.c_str());
    shader.setStrings(text, 1);

    auto includer = glslang::TShader::ForbidIncluder { };
    EShMessages messages = EShMsgDefault;
    const int defaultVersion = 410;
    std::cout << "Start parsing " << path.c_str() << "...\n";
    bool result = shader.parse(GetResources(), defaultVersion, false, messages, includer);
    std::cout << "Result: " << result << "\n";

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