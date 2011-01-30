#ifndef COMMON_SERIALIZATION_H
#define COMMON_SERIALIZATION_H

/*
* Include this file if you want to enable the serialization functionality which is disabled by default (see Common.h file).
* This file has to be included before including any serializable classes.
*/

#ifdef BOOST_CLASS_IMPLEMENTATION
#undef BOOST_CLASS_IMPLEMENTATION
#endif

#ifdef BOOST_CLASS_EXPORT
#undef BOOST_CLASS_EXPORT
#endif

#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/level.hpp>
#include <boost/serialization/export.hpp>

// We explicitly include the binary archives here to make sure all serializable classes include them.
// This is needed to make the serialization export work for the derived classes.
// If later we need to serialize other types of archive they must be included here as well.
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#endif // COMMON_SERIALIZATION_H