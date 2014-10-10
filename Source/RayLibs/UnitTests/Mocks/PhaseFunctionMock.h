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

#ifndef PHASE_FUNCTION_MOCK_H
#define PHASE_FUNCTION_MOCK_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/Core/PhaseFunction.h>

/*
FunctionMock mock implementation. Always returns constant value.
*/
class PhaseFunctionMock: public PhaseFunction
  {
  public:
    double ScatteringPDF(const Vector3D_d &i_incoming, const Vector3D_d &i_outgoing) const
      {
      return 1.0/(4.0*M_PI);
      }
  };

#endif // PHASE_FUNCTION_MOCK_H