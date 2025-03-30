#include "enjam/asset.h"
#include "enjam/assetfile_parser.h"
#include "enjam/asset_manager.h"
#include "enjam/log.h"
#include "enjam/program.h"

int main() {
  using namespace Enjam;

  auto file = "float: 3.14,\n"
              " int  : -42,\n"
              " uint  : 42,\n"
              " str1 : \" this is str \","
              " nested: { first: \" first property of nested object \" }";

  std::stringstream input { file };
  Asset asset;
  bool succeed = AssetFileParser::parse(input, asset);
  assert(succeed);

  assert(asset["float"].as<float_t>() == 3.14f);
  assert(asset["float"].as<double>() == 3.14f);

  assert(asset["int"].as<int64_t>() == -42);
  assert(asset["int"].as<int>() == -42);
  assert(asset["uint"].as<unsigned int>() == 42);

  assert(asset.at("nested")->is<Enjam::object_t>());
  assert(!asset.at("nested")->at("first")->as<std::string>().empty());

}