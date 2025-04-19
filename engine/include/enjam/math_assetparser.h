#ifndef INCLUDE_ENJAM_MATH_ASSETPARSER_H_
#define INCLUDE_ENJAM_MATH_ASSETPARSER_H_

#include <enjam/asset.h>
#include <enjam/math.h>

namespace Enjam {

template<>
struct AssetParser<math::mat4f> {
  static void fromAsset(const Asset& asset, math::mat4f& val) {
    auto i = 0;
    for (auto& aVal: asset) {
      val[i >> 2][i % 4] = aVal.as<float>();
      i++;
    }
  }

  static void toAsset(Asset& asset, const math::mat4f& val) {
    for (auto r = 0; r < val.NUM_ROWS; r++) {
      for (auto c = 0; c < val.NUM_COLS; c++) {
        asset.pushBack(val[r][c]);
      }
    }
  }
};

}

#endif //INCLUDE_ENJAM_MATH_ASSETPARSER_H_
