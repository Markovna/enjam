#include <enjam/Test.h>
#include <enjam/log.h>

namespace Enjam {

void Print() {
  EJ_INFO("This is info log {}", 42);
  EJ_ERROR("This is error {}", 42);
  EJ_WARN("This is warning log {}", 42);
  EJ_DEBUG("This is debug log {}", 42);
}

}
