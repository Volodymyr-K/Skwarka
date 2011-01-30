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

// Forward declaration of the "access" class that is needed for the boost serialization framework.
namespace boost {namespace serialization {class access;}}

/*
The BOOST_CLASS_IMPLEMENTATION and BOOST_CLASS_EXPORT macroses are defined in the boost serialization framework.
We don't include the framework headers by default because they drastically increase the linking time and the size of the resulting binaries.
To avoid compilation errors and to "disable" the serialization we define void implementation of these macroses.
The macroses are defined only if they are not yet defined in which case we do nothing.
If you need to "enable" the serialization you have to include the CommonSerialization.h file.
*/
#ifndef BOOST_CLASS_IMPLEMENTATION
#define BOOST_CLASS_IMPLEMENTATION(a, b) ;
#endif

#ifndef BOOST_CLASS_EXPORT
#define BOOST_CLASS_EXPORT(a) ;
#endif

#endif // COMMON_H