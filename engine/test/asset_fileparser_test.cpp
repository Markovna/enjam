#include "enjam/assets_manager.h"
#include "enjam/assetfile_parser.h"
#include "enjam/log.h"

int main() {
  using namespace Enjam;

  auto file = "float: 3.14,\n"
              " int  : -42,\n"
              " str1 : \" this is str \","
              " nested: { first: \" first property of nested object \" }";

  std::stringstream input { file };
  Asset asset;
  bool succeed = AssetFileParser::parse(input, asset);
  assert(succeed);
  std::hash<std::string> hash;

  assert(asset.isType<std::float_t>(hash("float")));
  assert(asset.get<std::float_t>(hash("float")) == 3.14f);

  assert(asset.isType<std::int64_t>(hash("int")));
  assert(asset.get<std::int64_t>(hash("int")) == -42);

  assert(asset.get<Asset>(hash("nested")));
  assert(!asset.get<Asset>(hash("nested"))->get<std::string>(hash("first")).empty());
  assert(asset.isType<Asset>(hash("nested")));
}