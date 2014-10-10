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