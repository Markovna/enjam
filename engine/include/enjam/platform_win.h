#pragma once

#include <enjam/defines.h>

#ifdef ENJAM_PLATFORM_WINDOWS

namespace Enjam {

using Platform = class PlatformWindows;

class Input;

class ENJAM_API PlatformWindows {
 public:
  void init();
  void shutdown();
};

}

#endif