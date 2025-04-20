#ifndef ENGINE_INCLUDE_ENJAM_DCC_ASSET_H_
#define ENGINE_INCLUDE_ENJAM_DCC_ASSET_H_

#include <vector>
#include <enjam/assets_manager.h>
#include <enjam/math.h>
#include <enjam/math_assetparser.h>

namespace Enjam {

class DCCAsset {
 public:
  struct Mesh {
    uint32_t offset;
    uint32_t count;
  };

  struct Node {
    math::mat4f transform;
    std::vector<Mesh> meshes;
  };

 public:
  explicit DCCAsset(
      std::vector<Node> nodes,
      std::vector<uint32_t> indices,
      std::vector<math::vec3f> positions,
      std::vector<math::vec2f> texCoords0,
      std::vector<math::vec2f> texCoords1) :
      nodes(std::move(nodes)),
      indices(std::move(indices)),
      positions(std::move(positions)),
      texCoords0(std::move(texCoords0)),
      texCoords1(std::move(texCoords1))
  { }

  const std::vector<Node>& getNodes() { return nodes; }
  const std::vector<uint32_t>& getIndices() { return indices; }
  const std::vector<math::vec3f>& getPositions() { return positions; }
  const std::vector<math::vec2f>& getTexCoords0() { return texCoords0; }
  const std::vector<math::vec2f>& getTexCoords1() { return texCoords1; }

 private:
  std::vector<Node> nodes;
  std::vector<uint32_t> indices;
  std::vector<math::vec3f> positions;
  std::vector<math::vec2f> texCoords0;
  std::vector<math::vec2f> texCoords1;
};

class DCCAssetFactory {
 private:
  template<class T>
  std::vector<T> copyBuffer(ByteArray buffer) {
    return utils::reinterpret_copy<T>(buffer.data(), buffer.size());
  }

 public:
  AssetRef<DCCAsset> operator()(const Asset& asset) {
    auto indices = copyBuffer<uint32_t>(asset.at("indices")->loadBuffer());
    auto positions = copyBuffer<math::vec3f>(asset.at("positions")->loadBuffer());
    auto texCoords0 = copyBuffer<math::vec2f>(asset.at("texCoords0")->loadBuffer());
    auto texCoords1 = copyBuffer<math::vec2f>(asset.at("texCoords1")->loadBuffer());

    std::vector<DCCAsset::Node> nodes;
    for (auto& assetNode: *asset.at("nodes")) {
      DCCAsset::Node node {
          .transform = assetNode.at("transform")->as<math::mat4f>(),
      };
      for (auto& assetMesh: *assetNode.at("meshes")) {
        node.meshes.push_back({
            .offset = assetMesh.at("offset")->as<uint32_t>(),
            .count = assetMesh.at("count")->as<uint32_t>(),
        });
      }
      nodes.push_back(std::move(node));
    }

    return std::make_shared<DCCAsset>(
        std::move(nodes),
        std::move(indices),
        std::move(positions),
        std::move(texCoords0),
        std::move(texCoords1));
  }
};

}

#endif //ENGINE_INCLUDE_ENJAM_DCC_ASSET_H_
