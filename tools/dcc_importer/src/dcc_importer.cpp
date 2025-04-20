#include <filesystem>
#include <string_view>
#include <vector>
#include <unordered_set>
#include <enjam/math_assetparser.h>
#include <enjam/asset.h>
#include <enjam/assets_repository.h>
#include <enjam/log.h>
#include <enjam/math.h>
#include <enjam/utils.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

using namespace Enjam;
using namespace Enjam::math;

class DCCImporter {
 public:
  explicit DCCImporter(const std::filesystem::path& inputPath);

  void operator()(Asset& asset) const {
    asset["indices"] = makeByteArray(data.indices.begin(), data.indices.end());
    asset["positions"] = makeByteArray(data.positions.begin(), data.positions.end());
    asset["texCoords0"] = makeByteArray(data.texCoords0.begin(), data.texCoords0.end());
    asset["texCoords1"] = makeByteArray(data.texCoords1.begin(), data.texCoords1.end());

    asset["nodes"] = Asset::array();
    auto& nodesAsset =  asset["nodes"];
    for(auto& node : data.nodes) {
      Asset nodeAsset;
      nodeAsset["name"] = node.name;
      nodeAsset["parent"] = node.parentIndex;
      nodeAsset["transform"] = node.transform;

      nodeAsset["meshes"] = Asset::array();
      auto& meshesAsset = nodeAsset["meshes"];
      for(auto& mesh : node.meshes) {
        Asset meshAsset;
        meshAsset["offset"] = mesh.offset;
        meshAsset["count"] = mesh.count;
        meshesAsset.pushBack(std::move(meshAsset));
      }

      nodesAsset.pushBack(std::move(nodeAsset));
    }
  }

 private:
  void processNode(const aiScene*, const aiNode*, int32_t parentIndex = -1);

  struct Mesh {
    uint64_t offset;
    uint64_t count;
  };

  struct Node {
    std::string name;
    int32_t parentIndex;
    Enjam::math::mat4f transform;
    std::vector<Mesh> meshes;
  };

  struct ImportedData {
    std::vector<vec3f> positions;
    std::vector<vec2f> texCoords0;
    std::vector<vec2f> texCoords1;
    std::vector<uint32_t> indices;

    std::vector<Node> nodes;
  };

  ImportedData data;
};

DCCImporter::DCCImporter(const std::filesystem::path& path) {
  Assimp::Importer importer;
  importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE | aiPrimitiveType_POINT);
  importer.SetPropertyBool(AI_CONFIG_IMPORT_COLLADA_IGNORE_UP_DIRECTION, true);
  importer.SetPropertyBool(AI_CONFIG_PP_PTV_KEEP_HIERARCHY, true);

  aiScene const* scene = importer.ReadFile(path.c_str(),
       // normals and tangents
       aiProcess_GenSmoothNormals |
       aiProcess_CalcTangentSpace |
       // UV Coordinates
       aiProcess_GenUVCoords |
       // topology optimization
       aiProcess_FindInstances |
       aiProcess_OptimizeMeshes |
       aiProcess_JoinIdenticalVertices |
       // misc optimization
       aiProcess_ImproveCacheLocality |
       aiProcess_SortByPType |
       // we only support triangles
       aiProcess_Triangulate);

  processNode(scene, scene->mRootNode);
}

void DCCImporter::processNode(const aiScene* scene, const aiNode* node, int32_t parentIndex) {
  auto& t = node->mTransformation;

  data.nodes.push_back({
       .name = node->mName.C_Str(),
       .parentIndex = parentIndex,
       .transform = transpose(mat4f {
           t.a1, t.a2, t.a3, t.a4,
           t.b1, t.b2, t.b3, t.b4,
           t.c1, t.c2, t.c3, t.c4,
           t.d1, t.d2, t.d3, t.d4
       })
   });


  for (size_t i = 0; i < node->mNumMeshes; i++) {
    aiMesh const* mesh = scene->mMeshes[node->mMeshes[i]];

    auto positions = reinterpret_cast<vec3f const*>(mesh->mVertices);
    auto texCoords0 = reinterpret_cast<vec3f const*>(mesh->mTextureCoords[0]);
    auto texCoords1 = reinterpret_cast<vec3f const*>(mesh->mTextureCoords[1]);

    const size_t numVertices = mesh->mNumVertices;

    if (numVertices > 0) {
      const aiFace* faces = mesh->mFaces;
      const size_t numFaces = mesh->mNumFaces;

      if (numFaces > 0) {
        size_t indicesOffset = data.positions.size();

        for (size_t j = 0; j < numVertices; j++) {
          vec2f texCoord0 = texCoords0 ? texCoords0[j].xy : vec2f { 0.0 };
          vec2f texCoord1 = texCoords1 ? texCoords1[j].xy : vec2f { 0.0 };

          data.texCoords0.emplace_back(texCoord0);
          data.texCoords1.emplace_back(texCoord1);
          data.positions.emplace_back(positions[j]);
        }

        size_t indicesCount = numFaces * faces[0].mNumIndices;
        size_t indexBufferOffset = data.indices.size();

        for (size_t j = 0; j < numFaces; ++j) {
          const aiFace& face = faces[j];
          for (size_t k = 0; k < face.mNumIndices; ++k) {
            data.indices.push_back(uint32_t(face.mIndices[k] + indicesOffset));
          }
        }

        data.nodes.back().meshes.push_back({
             .offset = indexBufferOffset,
             .count = indicesCount
         });
      }
    }
  }

  for(auto i = 0; i < node->mNumChildren; i++) {
    processNode(scene, node->mChildren[i], data.nodes.size() - 1);
  }
}

bool generateAsset(const std::filesystem::path& inputPath, const std::filesystem::path& outputPath) {
  using namespace Enjam;
  using namespace Assimp;

  const std::unordered_set<std::string> supportedExtensions {
      ".obj"
  };

  std::filesystem::path ext = inputPath.extension();
  if(supportedExtensions.find(ext.string()) == supportedExtensions.end()) {
    ENJAM_ERROR("Files with extension {} are not supported", ext.string());
    return false;
  }

  DCCImporter importer(inputPath);
  Asset asset;
  importer(asset);

  AssetsFilesystemRep repository;
  repository.save(outputPath, asset);
  return true;
}

int main(int argc, char* argv[]) {
  std::filesystem::path output;
  std::filesystem::path input;

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

    input = arg;
    it++;
  }

  if(input.empty()) {
    throw std::runtime_error("Input file path is not provided");
  }

  if(output.empty()) {
    output = input;
    output.replace_extension("nj_tex");
  }

  generateAsset(input, output);
}