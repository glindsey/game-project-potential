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

#ifndef NDEBUG

#define ASSERT_NOT_NULL(ptr)                                              \
{                                                                         \
  if (ptr == nullptr)                                                     \
  {                                                                       \
    FATAL_ERROR("Pointer %s is null!", ptrName);                          \
  }                                                                       \
}                                                                         \

#else
#define ASSERT_NOT_NULL(ptr)
#endif

#endif // ERRORMACROS_H_INCLUDED
