#ifndef INCLUDE_ENJAM_ASSERT_H_
#define INCLUDE_ENJAM_ASSERT_H_

#define ENJAM_ASSERT_VIOLATION_THROW

#include <enjam/log.h>

#define __ENJAM_ASSERT_FAILED_ERROR(exp) "Assertion failed " __ENJAM_LOG_S2(exp) " " __FILE__ ":" __ENJAM_LOG_S2(__LINE__)

#if defined(ENJAM_ASSERT_VIOLATION_THROW)
  #define ENJAM_ASSERT(exp)           \
  {                                   \
    if(exp) { }                       \
    else {                            \
      throw std::runtime_error(__ENJAM_ASSERT_FAILED_ERROR(exp)); \
    }                                 \
  }

#elif defined(ENJAM_ASSERT_VIOLATION_TERMINATE)
  #define ENJAM_ASSERT(exp)     \
  {                             \
    if(exp) { }                 \
    else {                      \
      ENJAM_ERROR(#exp);        \
      std::terminate();         \
    }                           \
  }

#else
  #define ENJAM_ASSERT(exp)

#endif

#endif //INCLUDE_ENJAM_ASSERT_H_
