#ifndef ASSERT_H
#define ASSERT_H

#include <crtdbg.h>

#ifdef NDEBUG
#define ASSERT(expr) ((void)0)
#else
#define ASSERT(expr) _ASSERTE(expr)
#endif // NDEBUG

#endif // ASSERT_H