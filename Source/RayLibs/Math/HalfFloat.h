/*
* Copyright (C) 2014 by Volodymyr Kachurovskyi <Volodymyr.Kachurovskyi@gmail.com>
*
* This file is part of Skwarka.
*
* Skwarka is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
*
* Skwarka is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with Skwarka.  If not, see <http://www.gnu.org/licenses/>.
*/

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