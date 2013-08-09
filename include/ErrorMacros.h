#ifndef ERRORMACROS_H_INCLUDED
#define ERRORMACROS_H_INCLUDED

#include "Settings.h"

#define MINOR_ERROR(...)                                                  \
{                                                                         \
  char buf[1024];                                                         \
  snprintf(buf, 1024, __VA_ARGS__);                                       \
  Settings::handleMinorError(buf, __FILE__, __LINE__, __func__);          \
}

#define MAJOR_ERROR(...)                                                  \
{                                                                         \
  char buf[1024];                                                         \
  snprintf(buf, 1024, __VA_ARGS__);                                       \
  Settings::handleMajorError(buf, __FILE__, __LINE__, __func__);          \
}

#define FATAL_ERROR(...)                                                  \
{                                                                         \
  char buf[1024];                                                         \
  snprintf(buf, 1024, __VA_ARGS__);                                       \
  Settings::handleFatalError(buf, __FILE__, __LINE__, __func__);          \
}

#define TRACE(...)                                                        \
{                                                                         \
  char buf[1024];                                                         \
  snprintf(buf, 1024, __VA_ARGS__);                                       \
  printf("%s:%d (%s): %s\n", __FILE__, __LINE__, __func__, buf);          \
}

#ifdef DEEP_TRACE
#define DEEP_TRACE(...) TRACE(...)
#else
#define DEEP_TRACE(...)
#endif

#define OUTPUT(...) printf(...)

#ifndef NDEBUG

#define ASSERT_NOT_NULL(ptr)                                              \
{                                                                         \
  if (ptr == nullptr)                                                     \
  {                                                                       \
    FATAL_ERROR("Pointer %s is null!", ##ptr);                            \
  }                                                                       \
}                                                                         \

#define ASSERT(condition)                                                 \
{                                                                         \
  if (!condition)                                                         \
  {                                                                       \
    FATAL_ERROR("Assertion failed: %s", ##condition);                     \
  }                                                                       \
}                                                                         \

#else
#define ASSERT_NOT_NULL(ptr)
#define ASSERT(condition)
#endif

#endif // ERRORMACROS_H_INCLUDED
