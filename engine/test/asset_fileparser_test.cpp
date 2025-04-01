#include <sstream>
#include "enjam/asset.h"
#include "enjam/assetfile_parser.h"

int main() {
  using namespace Enjam;

  auto file = "float: 3.14,\n"
              " int  : -42,\n"
              " uint  : 42,\n"
              " hexadecimal  : 0xdecaf,\n"
              " string : \"this is str \","
              " object: { first: \" first property of nested object \" },"
              " array: [ 1, \"str\", 0.42 ],"
              " property_name1     : 0    ";

  std::stringstream input { file };
  Asset asset;
  bool succeed = AssetFileParser::parse(input, asset);
  assert(succeed);

  assert(asset["float"].as<float_t>() == 3.14f);
  assert(asset["float"].as<double>() == 3.14f);

  assert(asset["int"].as<int64_t>() == -42);
  assert(asset["int"].as<int>() == -42);
  assert(asset["uint"].as<unsigned int>() == 42);

  assert(asset["hexadecimal"].is<Asset::int_t>());
  assert(asset["hexadecimal"].as<size_t>() == 912559);

  assert(asset.at("string"));
  assert(asset["string"].is<Asset::string_t>());

  assert(asset.at("object"));
  assert(asset.at("object")->is<Asset::object_t>());
  assert(!asset.at("object")->at("first")->as<std::string>().empty());

  assert(asset.at("array"));
  assert(asset.at("array")->is<Asset::array_t>());
  assert(asset["array"][0].is<Asset::int_t>());
  assert(asset["array"][1].is<Asset::string_t>());
  assert(asset["array"][2].is<Asset::float_t>());

  assert(asset.at("property_name1"));
}