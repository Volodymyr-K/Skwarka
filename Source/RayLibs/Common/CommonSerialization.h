#ifndef COMMON_SERIALIZATION_H
#define COMMON_SERIALIZATION_H

/*
* This file has to be included before including any serializable classes. By default, this is achieved by including Common.h, which in turn includes this file.
*/

#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/level.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/base_object.hpp>

// We explicitly include the binary archives here to make sure all serializable classes include them.
// This is needed to make the serialization export work for the derived classes.
// If later we need to serialize other types of archive they must be included here as well.
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#endif // COMMON_SERIALIZATION_H