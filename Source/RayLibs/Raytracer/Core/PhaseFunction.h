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

#ifndef PHASE_FUNCTION_H
#define PHASE_FUNCTION_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/Spectrum.h>

/**
* An abstract class defining the contract for phase functions.
* Defines a single method that returns probability density of light scattering for the specified incoming and outgoing directions.
* @sa VolumeRegion
*/
class PhaseFunction: public ReferenceCounted
  {
  public:
    /**
    * Returns probability density for the light to be scattered in the specified outgoing direction given the specified incoming direction.
    */
    virtual double ScatteringPDF(const Vector3D_d &i_incoming, const Vector3D_d &i_outgoing) const = 0;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* Serializes PhaseFunction to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void serialize(Archive &i_ar, PhaseFunction &i_phase_function, const unsigned int i_version)
  {
  // Nothing to do here, everything must be serialized by the derived classes.

  // Just call the serialization for the base ReferenceCounted class.
  i_ar & boost::serialization::base_object<ReferenceCounted>(i_phase_function);
  }

#endif // PHASE_FUNCTION_H