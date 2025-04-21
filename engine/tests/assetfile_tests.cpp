#include <sstream>
#include <iostream>
#include "enjam/asset.h"
#include "enjam/assetfile_reader.h"
#include "enjam/assetfile_writer.h"

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


  struct Input {
    std::istream& input;
    std::function<std::unique_ptr<std::istream>(uint64_t)> buffers;
  };

  std::stringstream strStream { file };
  Input input { .input = strStream, .buffers = { } };
  AssetFileReader reader { input };

  Asset asset;
  bool succeed = reader.parse(asset);
  assert(succeed);

  assert(asset["float"].as<float_t>() == 3.14f);
  assert(asset["float"].as<double>() == 3.14f);

  assert(asset["int"].as<int64_t>() == -42);
  assert(asset["int"].as<int>() == -42);
  assert(asset["uint"].as<unsigned int>() == 42);

  assert(asset["hexadecimal"].is<AssetType::BUFFER>());

  assert(asset.at("string"));
  assert(asset["string"].is<AssetType::STRING>());

  assert(asset.at("object"));
  assert(asset.at("object")->is<AssetType::OBJECT>());
  assert(!asset.at("object")->at("first")->as<std::string>().empty());

  assert(asset.at("array"));
  assert(asset.at("array")->is<AssetType::ARRAY>());
  assert(asset["array"][0].is<AssetType::INTEGER>());
  assert(asset["array"][1].is<AssetType::STRING>());
  assert(asset["array"][2].is<AssetType::FLOAT>());

  auto i = 0;
  for(auto& desc : asset["array"]) {
    if(i == 0) {
      assert(desc.is<AssetType::INTEGER>());
    } else if (i == 1) {
      assert(desc.is<AssetType::STRING>());
    } else if (i == 2) {
      assert(desc.is<AssetType::FLOAT>());
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

  struct Output {
    std::ostream& out;
    std::function<std::unique_ptr<std::ostream>(uint64_t)> buffers;
  };

  Output output {
    .out = std::cout,
    .buffers = { }
  };
  AssetFileWriter {
    output,
    AssetFileWriterFlags::pretty | AssetFileWriterFlags::omitFirstEnclosure
  }.write(newAsset);
  std::cout << "\n";

  static_assert(std::is_same_v<AssetIterator<Asset>::reference, Asset&>);
  static_assert(std::is_same_v<AssetIterator<const Asset>::reference, const Asset&>);
  static_assert(std::is_same_v<AssetIterator<Asset>::pointer, Asset*>);
  static_assert(std::is_same_v<AssetIterator<const Asset>::pointer, const Asset*>);

}