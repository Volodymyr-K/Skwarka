#ifndef ASSERT_H
#define ASSERT_H

#include <crtdbg.h>
#include "Log.h"

#ifdef NDEBUG
#define ASSERT(expr) ((void)0)
#else
#define ASSERT(expr) \
  { ((expr) ? (void)0 : Log::Warning("Assertion \"%s\" failed in %s, line %d", #expr, __FILE__, __LINE__)); _ASSERTE(expr); }
#endif // NDEBUG

#endif // ASSERT_H