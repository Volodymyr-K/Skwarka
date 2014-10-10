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

#ifndef RAYLEIGH_PHASE_FUNCTION_H
#define RAYLEIGH_PHASE_FUNCTION_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/Core/PhaseFunction.h>

/**
* Phase function implementing the Rayleigh scattering.
* Rayleigh scattering is observed on particles only slightly smaller than the wavelength of light (cigarette smoke and dust).
*/
class RayleighPhaseFunction: public PhaseFunction
  {
  public:
    /**
    * Returns probability density for the light to be scattered in the specified outgoing direction given the specified incoming direction.
    */
    double ScatteringPDF(const Vector3D_d &i_incoming, const Vector3D_d &i_outgoing) const;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline double RayleighPhaseFunction::ScatteringPDF(const Vector3D_d &i_incoming, const Vector3D_d &i_outgoing) const
  {
  double costheta = i_incoming*i_outgoing;
  return  3.0/(16.0*M_PI) * (1.0 + costheta * costheta);
  }

/**
* Serializes RayleighPhaseFunction to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void serialize(Archive &i_ar, RayleighPhaseFunction &i_phase_function, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<PhaseFunction>(i_phase_function);
  }

// Register the derived class in the boost serialization framework.
BOOST_CLASS_EXPORT_KEY(RayleighPhaseFunction)

#endif // RAYLEIGH_PHASE_FUNCTION_H