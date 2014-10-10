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

#ifndef CORE_COMMON_H
#define CORE_COMMON_H

#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include <Math/RandomGenerator.h>

/**
* This structure encapsulates objects that are passed through the pipeline of the raytracer and which are specific to each thread.
*/
struct ThreadSpecifics
  {
  MemoryPool *mp_pool;

  RandomGenerator<double> *mp_random_generator;
  };

#endif // CORE_COMMON_H