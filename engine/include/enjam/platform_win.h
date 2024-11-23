#pragma once

#include <enjam/defines.h>

#ifdef ENJAM_PLATFORM_WINDOWS

namespace Enjam {

using Platform = class PlatformWindows;

class Engine;

class ENJAM_API PlatformWindows {
 public:
  void init(Engine& engine);
  void shutdown();
};

}

#endif