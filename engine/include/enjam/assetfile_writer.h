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
  explicit AssetFileWriter(TOutput& output, fmtflag_t flags = 0)
      : output(output), flags(flags) {}

  void write(const Asset&, uint intent = 0);

  void operator()(const Asset& asset) {
    write(asset);
  }

 private:
  void printIntent(uint intent);
  void printNextLine();

  void printArray(Asset::const_iterator, Asset::const_iterator, uint intent);
  void printObject(Asset::const_iterator, Asset::const_iterator, uint intent);

 private:
  TOutput& output;
  fmtflag_t flags;
};

template<class TOutput>
void AssetFileWriter<TOutput>::write(const Asset& asset, uint intent) {
  switch(asset.type()) {
    case AssetType::INTEGER: {
      output.out << asset.as<int64_t>();
      break;
    }
    case AssetType::FLOAT: {
      output.out << asset.as<float>();
      break;
    }
    case AssetType::STRING: {
      output.out << '"' << asset.as<std::string>() << '"';
      break;
    }
    case AssetType::ARRAY: {
      printArray(asset.begin(), asset.end(), intent);
      break;
    }
    case AssetType::OBJECT: {
      printObject(asset.begin(), asset.end(), intent);
      break;
    }
    case AssetType::BUFFER: {
      auto& bufferLoader = asset.getBufferLoader();
      auto buffer = bufferLoader();
      auto hash = std::hash<ByteArray>{}(buffer);

      std::vector<std::ostream::char_type> fileBuffer { buffer.begin(), buffer.end() };
      output.buffers(hash)->write(fileBuffer.data(), fileBuffer.size());

      output.out << fmt::format("{:#x}", hash);

      break;
    }
  }
}

template<class TWriter>
void AssetFileWriter<TWriter>::printIntent(uint intent) {
  if ((flags & AssetFileWriterFlags::pretty) == AssetFileWriterFlags::pretty) {
    for (int i = 0; i < intent; i++) {
      if (!(flags & AssetFileWriterFlags::omitFirstEnclosure) || i > 0)
        output.out << "    ";
    }
  }
}

template<class TWriter>
void AssetFileWriter<TWriter>::printNextLine() {
  if ((flags & AssetFileWriterFlags::pretty) == AssetFileWriterFlags::pretty) {
    output.out << "\n";
  }
}

template<class TWriter>
void AssetFileWriter<TWriter>::printObject(Asset::const_iterator begin, Asset::const_iterator end, uint intent) {
  if (!(flags & AssetFileWriterFlags::omitFirstEnclosure) || intent > 0) {
    output.out << '{';
  }
  size_t i = 0;
  intent++;
  for(auto it = begin; it != end; it++) {
    if(it != begin) {
      output.out << ',';
    }
    printNextLine();
    printIntent(intent);
    output.out << it.key() << ": ";
    write(*it, intent);
    i++;
  }
  intent--;

  if(begin != end) {
    printNextLine();
  }

  if (!(flags & AssetFileWriterFlags::omitFirstEnclosure) || intent > 0) {
    printIntent(intent);
    output.out << '}';
  }
}

template<class TWriter>
void AssetFileWriter<TWriter>::printArray(Asset::const_iterator begin, Asset::const_iterator end, uint intent) {
  bool flat = true;
  if (flags & AssetFileWriterFlags::pretty) {
    flat &= !std::any_of(begin, end, [](auto& v) { return !v.isNumeric(); });
  }

  output.out << '[';
  intent++;

  for (auto it = begin; it != end; it++) {
    if(it != begin) {
      output.out << ',';
    }
    if (!flat) {
      printNextLine();
      printIntent(intent);
    }
    auto& item = *it;
    write(item, flat ? 0 : intent);
  }

  intent--;

  if(!flat && begin != end) {
    printNextLine();
    printIntent(intent);
  }

  output.out << ']';
}

}

#endif //INCLUDE_ENJAM_ASSETFILE_SERIALIZER_H_
