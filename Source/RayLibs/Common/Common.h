#ifndef COMMON_H
#define COMMON_H

/*
* This file is a convenient header including and defining the most common stuff that is used almost everywhere (e.g. assert, shared_ptr etc.).
*/

#include "IntrusiveSmartPointer.h"
#include "Assert.h"
#include "Log.h"
#include "Numerics.h"

#include <memory>
using std::shared_ptr;

#include "CommonSerialization.h"

#endif // COMMON_H