#include <enjam/assetfile_serializer.h>
#include <enjam/asset.h>
#include <enjam/type_traits_helpers.h>

namespace Enjam {

void AssetFileSerializer::operator()(const Asset& asset, std::ostream& out) {
  dump(asset, out);
}

void AssetFileSerializer::dump(const Asset& asset, std::ostream& out, uint intent) {
  asset.visit(overloaded {
      [&out](auto& val) { out << val; },
      [&out](const Asset::string_t& val) {
        out << "\"" << val << "\"";
      },
      [&out, this, intent](const Asset::array_t& val) mutable {
        out << '[';
        printNextLine(out);
        intent++;
        for(auto i = 0 ; i < val.size(); i++) {
          auto& item = val[i];
          dump(item, out, intent);
          if(i < val.size() - 1) {
            out << ',';
          }
          printNextLine(out);
        }
        intent--;
        printIntent(out, intent);
        out << ']';
      },
      [&out, this, intent](const Asset::object_t& val) mutable {
        if(!(flags & omitFirstEnclosure) || intent > 0) {
          out << '{';
          printNextLine(out);
        }
        size_t i = 0;
        intent++;
        for(auto& prop : val) {
          printIntent(out, intent);
          out << prop.name << ": ";
          dump(prop.value, out, intent);
          if(i < val.size() - 1) {
            out << ',';
          }
          printNextLine(out);
          i++;
        }
        intent--;
        if(!(flags & omitFirstEnclosure) || intent > 0) {
          printIntent(out, intent);
          out << '}';
        }
      }
  });
}

void AssetFileSerializer::printIntent(std::ostream& out, uint intent) {
  if((flags & pretty) == pretty) {
    for(int i = 0; i < intent; i++) {
      if(!(flags & omitFirstEnclosure) || i > 0)
      out << "    ";
    }
  }
}

void AssetFileSerializer::printNextLine(std::ostream& out) {
  if((flags & pretty) == pretty) {
    out << "\n";
  }
}

}