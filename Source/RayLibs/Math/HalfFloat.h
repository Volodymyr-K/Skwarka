#ifndef HALF_FLOAT_H
#define HALF_FLOAT_H

// Disable compiler warning raised in the OpenEXR's code.
#pragma warning( push )
#pragma warning( disable : 4231 )
#include <half.h>
#pragma warning( pop )

typedef half HalfFloat;

template<class Archive>
void save(Archive &i_ar, const HalfFloat &i_half, const unsigned int i_version)
  {
  unsigned short bits = i_half.bits();
  i_ar << bits;
  }

template<class Archive>
void load(Archive &i_ar, HalfFloat &i_half, const unsigned int i_version)
  {
  unsigned short bits;
  i_ar >> bits;
  i_half.setBits(bits);
  }

template<class Archive>
void serialize(Archive &i_ar, HalfFloat &i_half, const unsigned int i_version)
  {
  boost::serialization::split_free(i_ar, i_half, i_version);
  }

// Don't store class info for HalfFloat.
BOOST_CLASS_IMPLEMENTATION(HalfFloat, boost::serialization::object_serializable)

#endif // HALF_FLOAT_H