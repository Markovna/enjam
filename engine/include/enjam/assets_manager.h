#ifndef INCLUDE_ENJAM_RESOURCE_CACHE_H_
#define INCLUDE_ENJAM_RESOURCE_CACHE_H_

#include <filesystem>
#include <memory>
#include <unordered_map>
#include <map>
#include <utility>
#include <enjam/assets_repository.h>

/*
 *
 * To hold dependencies of an asset we use the aliasing constructor of the shared_ptr
 * https://youtu.be/l6Y9PqyK1Mc?t=2144
 * https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
 *
 *   struct Holder {
 *     std::vector<ResourceRef<Texture>> textureRefs;
 *     Material material;
 *   };
 *
 *   ...
 *
 *   auto holderPtr = std::make_shared<Holder>(holder);
 *   auto ptr = std::shared_ptr<Material>(holderPtr, &holderPtr->material);
 *
 */

// TODO:
//
// 1. Write tests for Assets Manager
//
// 2. Implement our own smart pointer for Resource instead of shared_ptr. Should be able to
//    hold other dependencies as references. Also will give more control on how resources
//    are aligned in memory.

namespace Enjam {

class Asset;
class AssetsRepository;

template<class TAsset>
using AssetRef = std::shared_ptr<const TAsset>;

template<class TAsset>
class AssetsCache {
 public:
  AssetRef<TAsset> find(const std::filesystem::path& path) {
    AssetRef<TAsset> ref;

    auto it = cacheByPath.find(path);
    if(it != cacheByPath.end()) {
      ref = it->second.lock();
    }

    return ref;
  }

  std::weak_ptr<const TAsset>& operator[](const std::filesystem::path& path) {
    return cacheByPath[path];
  }

 private:
  std::unordered_map<std::string, std::weak_ptr<const TAsset>> cacheByPath;
};

template<class T>
class AssetBuilder;

template<class TAsset>
class AssetsManager {
 public:
  using AssetRef = AssetRef<TAsset>;
  using Path = std::filesystem::path;
  using AssetRepository = std::function<AssetsRepository::Ref(const Path&)>;

  explicit AssetsManager(AssetRepository assetRepository)
    : cache(), assetRepository(std::move(assetRepository))
  { }

  template<class ...Args>
  AssetRef load(const Path& path, Args&&... args) {
    AssetRef ref = cache.find(path);

    if(!ref) {
      auto assetRoot = assetRepository(path);
      AssetBuilder<TAsset> builder(*assetRoot);
      ref = builder(std::forward<Args>(args)...);
      cache[path] = ref;
    }

    return ref;
  }

 private:
  using Cache = AssetsCache<TAsset>;

  Cache cache;
  AssetRepository assetRepository;
};

}

#endif // INCLUDE_ENJAM_RESOURCE_CACHE_H_
