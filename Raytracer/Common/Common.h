#ifndef COMMON_H
#define COMMON_H

/*
* This file is a convenient header including and defining the most common stuff that is used almost everywhere (e.g. assert).
*/

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