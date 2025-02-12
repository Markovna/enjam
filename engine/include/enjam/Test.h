#pragma once

#include "defines.h"
#include <functional>

namespace Enjam {

using SetupCallback = std::function<void()>;

ENJAM_API void Test(int argc, char* argv[]);

}