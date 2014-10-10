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

#ifndef MITCHELL_FILTER_H
#define MITCHELL_FILTER_H

#include <Raytracer/Core/FilmFilter.h>

/**
* Mitchell filter implementation. Mitchell filter does a relatively good job of trading off between ringing (phantom edges next to actual edges in the image) and blurring.
* Mitchell filter has negative lobes and thus special care needs to be taken to make sure the final filtered result is in the correct range.
*/
class MitchellFilter: public FilmFilter
  {
  public:
    /**
    * Creates MitchellFilter with the specified width and B and C parameters.
    * B and C parameters control the behavior of the filter by choosing between ringing, blurring and anisotropy.
    * Values of 1/3 for both parameters are suitable for most of the cases.
    */
    MitchellFilter(double i_x_width, double i_y_width, double i_b, double i_c);

    /**
    * Creates MitchellFilter with the specified width and B parameter.
    * Parameter C of the Mitchell filter is automatically selected to give best results.
    * Smaller value of B lead to blurring while higher values lead to ringing.
    * Values of 1/3 is suitable for most of the cases.
    */
    MitchellFilter(double i_x_width, double i_y_width, double i_b = 1.0/3.0);

    /**
    * Returns the filter value for the specified X and Y coordinates.
    */
    double Evaluate(double i_x, double i_y) const;

    /**
    * Gets B and C parameters of the filter.
    */
    void GetParameters(double &o_b, double &o_c) const;

  private:
    MitchellFilter() {}; // Empty default constructor for the boost serialization framework.

    // Needed for the boost serialization framework.  
    friend class boost::serialization::access;

    /**
    * Serializes to/from the specified Archive. This method is used by the boost serialization framework.
    */
    template<class Archive>
    void serialize(Archive &i_ar, const unsigned int i_version);

  private:
    /**
    * Private helper method that computes filter value for 1D.
    */
    double _Mitchell1D(double i_x) const;

  private:
    double m_b, m_c;
    double m_inv_x_width, m_inv_y_width;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline double MitchellFilter::_Mitchell1D(double i_x) const
  {
  i_x = fabs(2.0 * i_x);
  if (i_x > 1.0)
    return ((-m_b - 6*m_c) * i_x*i_x*i_x + (6*m_b + 30*m_c) * i_x*i_x +
    (-12*m_b - 48*m_c) * i_x + (8*m_b + 24*m_c)) * (1.0/6.0);
  else
    return ((12 - 9*m_b - 6*m_c) * i_x*i_x*i_x +
    (-18 + 12*m_b + 6*m_c) * i_x*i_x +
    (6 - 2*m_b)) * (1.0/6.0);
  }

template<class Archive>
void MitchellFilter::serialize(Archive &i_ar, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<FilmFilter>(*this);
  i_ar & m_b;
  i_ar & m_c;
  i_ar & m_inv_x_width;
  i_ar & m_inv_y_width;
  }

// Register the derived class in the boost serialization framework.
BOOST_CLASS_EXPORT_KEY(MitchellFilter)

#endif // MITCHELL_FILTER_H