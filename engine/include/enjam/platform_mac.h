#pragma once

#include <enjam/defines.h>
#include <vector>

#ifdef ENJAM_PLATFORM_DARWIN

namespace Enjam {

using Platform = class PlatformMac;

class Engine;

class ENJAM_API PlatformMac {
 public:
  void init(Engine& engine);
  void pollInputEvents();
  void shutdown();
};

}

#endif