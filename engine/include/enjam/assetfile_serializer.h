#ifndef INCLUDE_ENJAM_ASSETFILE_SERIALIZER_H_
#define INCLUDE_ENJAM_ASSETFILE_SERIALIZER_H_

#include <ostream>

namespace Enjam {

class Asset;

class AssetFileSerializer {
 public:
  using fmtflags = uint;

  static constexpr fmtflags pretty = (1u << 0);
  static constexpr fmtflags omitFirstEnclosure = (1u << 1);

 public:
  void dump(const Asset&, std::ostream&, uint intent = 0);
  void operator()(const Asset&, std::ostream&);

  AssetFileSerializer& setFlags(fmtflags f) {
    flags = f;
    return *this;
  }

 private:
  void printIntent(std::ostream&, uint intent);
  void printNextLine(std::ostream&);

 private:
  fmtflags flags;
};

}

#endif //INCLUDE_ENJAM_ASSETFILE_SERIALIZER_H_
