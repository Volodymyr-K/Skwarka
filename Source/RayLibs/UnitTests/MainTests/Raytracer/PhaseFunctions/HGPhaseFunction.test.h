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

#ifndef HG_PHASE_FUNCTION_TEST_H
#define HG_PHASE_FUNCTION_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Math/Vector3D.h>
#include <Raytracer/PhaseFunctions/HGPhaseFunction.h>

class HGPhaseFunctionTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_HGPhaseFunction()
      {
      intrusive_ptr<const PhaseFunction> p_pf1(new HGPhaseFunction(-0.5));
      intrusive_ptr<const PhaseFunction> p_pf2(new HGPhaseFunction(0.0));
      intrusive_ptr<const PhaseFunction> p_pf3(new HGPhaseFunction(0.5));

      Vector3D_d incoming=Vector3D_d(1,2,3).Normalized();
      Vector3D_d outgoing=Vector3D_d(-1,2,3).Normalized();
      double value1 = p_pf1->ScatteringPDF(incoming, outgoing);
      double value2 = p_pf2->ScatteringPDF(incoming, outgoing);
      double value3 = p_pf3->ScatteringPDF(incoming, outgoing);

      // If the asymmetry parameter has value equal to zero the phase function is the isotropic one. 
      TS_ASSERT_DELTA(value2, 1.0/(4.0*M_PI), (1e-9));

      // Negative values of the asymmetry parameter correspond to back-scattering while positive values correspond to forward-scattering.
      // So the values should be ordered accordingly.
      TS_ASSERT(value1<value2 && value2<value3);
      }

  };

#endif // HG_PHASE_FUNCTION_TEST_H