#ifndef INCLUDE_ENJAM_ASSETFILE_SERIALIZER_H_
#define INCLUDE_ENJAM_ASSETFILE_SERIALIZER_H_

#include <enjam/asset.h>
#include <ostream>
#include <functional>

namespace Enjam {

class Asset;

struct AssetFileWriterFlags {
  using value_t = uint;

  static constexpr value_t pretty = (1u << 0);
  static constexpr value_t omitFirstEnclosure = (1u << 1);
};

template<class TOutput>
class AssetFileWriter {
 public:
  using fmtflag_t = AssetFileWriterFlags::value_t;

 public:
  explicit AssetFileWriter(TOutput&& output, fmtflag_t flags = 0)
    : output(std::move(output)), flags(flags) { }

  void write(const Asset&, uint intent = 0);

  void operator()(const Asset& asset) {
    write(asset);
  }

 private:
  void printIntent(uint intent);
  void printNextLine();

  void printObject(const Asset::object_t&, uint intent);
  void printArray(const Asset::array_t&, uint intent);

 private:
  TOutput output;
  fmtflag_t flags;
};

template<class TOutput>
void AssetFileWriter<TOutput>::write(const Asset& asset, uint intent) {
  asset.visit(overloaded {
      [this](auto& val) { output << val; },
      [this](const Asset::string_t& val) { output << "\"" << val << "\""; },
      [this, intent](const Asset::array_t& val) { printArray(val, intent); },
      [this, intent](const Asset::object_t& val) { printObject(val, intent); }
  });
}

template<class TWriter>
void AssetFileWriter<TWriter>::printIntent(uint intent) {
  if((flags & AssetFileWriterFlags::pretty) == AssetFileWriterFlags::pretty) {
    for(int i = 0; i < intent; i++) {
      if(!(flags & AssetFileWriterFlags::omitFirstEnclosure) || i > 0)
        output << "    ";
    }
  }
}

template<class TWriter>
void AssetFileWriter<TWriter>::printNextLine() {
  if((flags & AssetFileWriterFlags::pretty) == AssetFileWriterFlags::pretty) {
    output << "\n";
  }
}

template<class TWriter>
void AssetFileWriter<TWriter>::printObject(const Asset::object_t& val, uint intent) {
  if(!(flags & AssetFileWriterFlags::omitFirstEnclosure) || intent > 0) {
    output << '{';
    printNextLine();
  }
  size_t i = 0;
  intent++;
  for(auto& prop : val) {
    printIntent(intent);
    output << prop.name << ": ";
    write(prop.value, intent);
    if(i < val.size() - 1) {
      output << ',';
    }
    printNextLine();
    i++;
  }
  intent--;
  if(!(flags & AssetFileWriterFlags::omitFirstEnclosure) || intent > 0) {
    printIntent(intent);
    output << '}';
  }
}

template<class TWriter>
void AssetFileWriter<TWriter>::printArray(const Asset::array_t& val, uint intent) {
  bool flat = true;
  if(flags & AssetFileWriterFlags::pretty) {
    flat &= std::any_of(val.begin(), val.end(), [](auto& v) { return !v.isNumeric(); });
  }
  output << '[';
  if(!flat) {
    printNextLine();
  }
  intent++;

  for(auto i = 0 ; i < val.size(); i++) {
    auto& item = val[i];
    write(item, intent);
    if(i < val.size() - 1) {
      output << ',';
    }
    if(!flat) {
      printNextLine();
    }
  }
  intent--;
  printIntent(intent);
  output << ']';
}

}

#endif //INCLUDE_ENJAM_ASSETFILE_SERIALIZER_H_
