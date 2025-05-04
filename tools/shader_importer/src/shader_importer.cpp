#include <unordered_set>
#include <filesystem>
#include <sstream>
#include <enjam/assets_repository.h>
#include <glslang/Public/ShaderLang.h>
#include <glslang/Public/ResourceLimits.h>
#include <SPIRV/SpvTools.h>

using Path = std::filesystem::path;
using SpirvBlob = std::vector<uint32_t>;

bool OutputSpvBin(const SpirvBlob& spirv, std::ostream& out) {
  for (int i = 0; i < (int)spirv.size(); ++i) {
    unsigned int word = spirv[i];
    out.write((const char*)&word, 4);
  }
  return true;
}

bool OutputSpvBin(const SpirvBlob& spirv, Enjam::ByteArray& bytes) {
  bytes.resize(spirv.size() * sizeof(uint32_t));
  uint32_t* curr = (uint32_t*) bytes.data();
  for (size_t i = 0; i < spirv.size(); ++i) {
    *curr = spirv[i];
    curr++;
  }
  return true;
}

const char* getStageName(EShLanguage stage)
{
  const char* name;
  switch (stage) {
    case EShLangVertex:          name = "vert";    break;
    case EShLangTessControl:     name = "tesc";    break;
    case EShLangTessEvaluation:  name = "tese";    break;
    case EShLangGeometry:        name = "geom";    break;
    case EShLangFragment:        name = "frag";    break;
    case EShLangCompute:         name = "comp";    break;
    case EShLangRayGen:          name = "rgen";    break;
    case EShLangIntersect:       name = "rint";    break;
    case EShLangAnyHit:          name = "rahit";   break;
    case EShLangClosestHit:      name = "rchit";   break;
    case EShLangMiss:            name = "rmiss";   break;
    case EShLangCallable:        name = "rcall";   break;
    case EShLangMesh :           name = "mesh";    break;
    case EShLangTask :           name = "task";    break;
    default:                     name = "unknown";     break;
  }

  return name;
}

EShLanguage toShadingStage(const std::string& ext) {
  auto stageName = ext;
  if (stageName == ".vert")
    return EShLangVertex;
  else if (stageName == ".tesc")
    return EShLangTessControl;
  else if (stageName == ".tese")
    return EShLangTessEvaluation;
  else if (stageName == ".geom")
    return EShLangGeometry;
  else if (stageName == ".frag")
    return EShLangFragment;
  else if (stageName == ".comp")
    return EShLangCompute;
  else if (stageName == ".rgen")
    return EShLangRayGen;
  else if (stageName == ".rint")
    return EShLangIntersect;
  else if (stageName == ".rahit")
    return EShLangAnyHit;
  else if (stageName == ".rchit")
    return EShLangClosestHit;
  else if (stageName == ".rmiss")
    return EShLangMiss;
  else if (stageName == ".rcall")
    return EShLangCallable;
  else if (stageName == ".mesh")
    return EShLangMesh;
  else if (stageName == ".task")
    return EShLangTask;

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

    Source(const char* str, EShLanguage stage, const Path& path)
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

      delete shader;
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

    shader->setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_0);

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


  Asset asset;

  for(auto& source : sources) {
    glslang::SpvOptions options;
    options.generateDebugInfo = true;
    SpirvBlob spirvOutput;
    spv::SpvBuildLogger logger;
    glslang::GlslangToSpv(*program.getIntermediate((EShLanguage) source.stage), spirvOutput, &logger, &options);

    Enjam::ByteArray spvBytes;
    OutputSpvBin(spirvOutput, spvBytes);
    Asset sourceAsset = Asset::object();
    sourceAsset["spv"] = spvBytes;
    sourceAsset["glsl"] = makeByteArray(source.text, strlen(source.text));
    asset[getStageName(source.stage)] = sourceAsset;

    if(!logger.getAllMessages().empty()) {
      ENJAM_INFO("{}", logger.getAllMessages());
    }
  }

  AssetsFilesystemRep repository;
  repository.save(outputPath, asset);

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