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

#ifndef TEXTURE_MOCK_H
#define TEXTURE_MOCK_H

#include <Raytracer/Core/Texture.h>
#include <Raytracer/Core/DifferentialGeometry.h>

/*
Texture mock implementation.
Just like the ConstantTexture it returns the same value everywhere.
*/
template<typename T>
class TextureMock: public Texture<T>
  {
  public:
    TextureMock(const T &i_value);

    virtual T Evaluate(const DifferentialGeometry &i_dg, size_t i_triangle_index) const;

  private:
    T m_value;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
TextureMock<T>::TextureMock(const T &i_value):
Texture(), m_value(i_value)
  {
  }

template<typename T>
T TextureMock<T>::Evaluate(const DifferentialGeometry &i_dg, size_t i_triangle_index) const
  {
  return m_value;
  }

#endif // TEXTURE_MOCK_H