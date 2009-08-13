#ifndef COMMON_H
#define COMMON_H

#include <crtdbg.h>
#include <boost\shared_ptr.hpp>
#include "Log.h"

using boost::shared_ptr;

#ifdef NDEBUG
#define ASSERT(expr) ((void)0)
#else
#define ASSERT(expr) \
  { ((expr) ? (void)0 : Log::Warning("Assertion \"%s\" failed in %s, line %d", #expr, __FILE__, __LINE__)); _ASSERTE(expr); }
#endif // NDEBUG

#endif // COMMON_H