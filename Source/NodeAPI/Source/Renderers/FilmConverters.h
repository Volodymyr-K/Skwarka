/*
* Copyright (C) 2015 by Volodymyr Kachurovskyi <Volodymyr.Kachurovskyi@gmail.com>
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

#ifndef SKWARKA_NODE_RENDERER_H
#define SKWARKA_NODE_RENDERER_H

#include <Common/Common.h>
#include <Raytracer/Core/Film.h>

#include <vector>

namespace NodeAPI
  {
  namespace FilmConverters
    {
    std::vector<unsigned char> FilmToPNG(intrusive_ptr<const Film> ip_film);
    }
  }

#endif // SKWARKA_NODE_RENDERER_H