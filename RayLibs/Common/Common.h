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
#include <boost/serialization/level.hpp>

// We explicitly include the binary archives here to make sure all serializable classes include them.
// This is needed to make the serialization export work for the derived classes.
// If later we need to serialize other types of archive they must be included here as well.
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#endif // COMMON_H