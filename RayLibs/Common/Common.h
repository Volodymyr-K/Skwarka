#ifndef COMMON_H
#define COMMON_H

/*
* This file is a convenient header including and defining the most common stuff that is used almost everywhere (e.g. assert, shared_ptr etc.).
*/

#include "IntrusiveSmartPointer.h"
#include "Assert.h"
#include "Log.h"
#include "Numerics.h"

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>

#endif // COMMON_H