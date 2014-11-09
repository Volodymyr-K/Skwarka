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

#ifndef IMAGE_SOURCE_H
#define IMAGE_SOURCE_H

#include <Common/Common.h>
#include <vector>

/**
* An abstract class defining the contract for getting 2D array of values.
* Implementations of the class provide 2D array of values (i.e. image) of the specified type (see template parameter of the class).
* This allows for having a convenient way to convert 2D array of values of one type to another type.
* For example, the implementation can store image in RGB format where each component is defined by unsigend char and return the image in SpectrumCoef_f format.
*/
template <typename T>
class ImageSource: public ReferenceCounted
  {
  public:

    /**
    * Gets 2D array of values (image).
    */
    virtual std::vector<std::vector<T>> GetImage() const = 0;

    /**
    * Returns height of the image, i.e. the size of the outer vector which defines the image.
    */
    virtual size_t GetHeight() const = 0;

    /**
    * Returns width of the image, i.e. the size of the inner vector which defines the image.
    */
    virtual size_t GetWidth() const = 0;

    ~ImageSource();

  protected:
    ImageSource();

  public:
    // Not implemented, not a value type.
    ImageSource(const ImageSource&);
    ImageSource &operator=(const ImageSource&);
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
ImageSource<T>::ImageSource()
  {
  }

template <typename T>
ImageSource<T>::~ImageSource()
  {
  }

#endif // IMAGE_SOURCE_H