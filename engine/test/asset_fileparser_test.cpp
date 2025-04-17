#include <sstream>
#include <iostream>
#include "enjam/asset.h"
#include "enjam/assetfile_parser.h"
#include "enjam/assetfile_serializer.h"
#include "enjam/type_traits_helpers.h"

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
  AssetFileParser parser {input};
  bool succeed = parser(asset);
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

  auto i = 0;
  for(auto& desc : asset["array"]) {
    if(i == 0) {
      assert(desc.is<Asset::int_t>());
    } else if (i == 1) {
      assert(desc.is<Asset::string_t>());
    } else if (i == 2) {
      assert(desc.is<Asset::float_t>());
    }
    i++;
  }

  assert(asset.at("property_name1"));

  Asset newAsset;
  newAsset["float"] = 3.14;
  newAsset["int"] = -42;
  newAsset["uint"] = 42;

  std::string str("this is str ");
  newAsset["str"] = str;

  Asset& nested = newAsset["object"];
  nested["first"] = " first property of nested object ";

  Asset& nested2 = nested["second"];
  nested2["second_0"] = "we need to go deeper";
  nested2["second_1"] = 0;

  AssetFileSerializer()(newAsset, std::cout);
  std::cout << "\n";
  AssetFileSerializer(AssetFileSerializer::pretty | AssetFileSerializer::omitFirstEnclosure)(newAsset, std::cout);

  static_assert(std::is_same_v<AssetIterator<Asset>::reference, Asset&>);
  static_assert(std::is_same_v<AssetIterator<const Asset>::reference, const Asset&>);
  static_assert(std::is_same_v<AssetIterator<Asset>::pointer, Asset*>);
  static_assert(std::is_same_v<AssetIterator<const Asset>::pointer, const Asset*>);

}