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

#ifndef BOX_FILTER_H
#define BOX_FILTER_H

#include <Raytracer/Core/FilmFilter.h>

/**
* Simplest implementation of the FilmFilter interface returning a constant value everywhere inside the filter's width.
*/
class BoxFilter: public FilmFilter
  {
  public:
    BoxFilter(double i_x_width, double i_y_width);

    /**
    * Returns value 1.0 everywhere inside the filter's width.
    */
    double Evaluate(double i_x, double i_y) const;

  private:
    BoxFilter() {}; // Empty default constructor for the boost serialization framework.

    // Needed for the boost serialization framework.  
    friend class boost::serialization::access;

    /**
    * Serializes to/from the specified Archive. This method is used by the boost serialization framework.
    */
    template<class Archive>
    void serialize(Archive &i_ar, const unsigned int i_version);
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class Archive>
void BoxFilter::serialize(Archive &i_ar, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<FilmFilter>(*this);
  }

// Register the derived class in the boost serialization framework.
BOOST_CLASS_EXPORT_KEY(BoxFilter)

#endif // BOX_FILTER_H