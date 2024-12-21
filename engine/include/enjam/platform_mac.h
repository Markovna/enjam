#pragma once

#include <enjam/defines.h>
#include <vector>

#ifdef ENJAM_PLATFORM_DARWIN

namespace Enjam {

using Platform = class PlatformMac;

class Input;

class ENJAM_API PlatformMac {
 public:
  explicit PlatformMac(Input& input);
  void init();
  void pollInputEvents();
  void shutdown();
};

}

#endif