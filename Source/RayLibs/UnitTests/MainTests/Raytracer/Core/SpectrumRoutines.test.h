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

#ifndef SPECTRUM_ROUTINES_TEST_H
#define SPECTRUM_ROUTINES_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/Core/SpectrumRoutines.h>
#include <limits>

class SpectrumRoutinesTestSuite : public CxxTest::TestSuite
  {
  public:

    void test_SpectrumRoutines_SpectrumToXYZ()
      {
      Spectrum_d s(2.0,2.0,2.0);
      XYZColor_d xyz = SpectrumRoutines::SpectrumToXYZ(s);
      TS_ASSERT(xyz[0]>=0.0);
      TS_ASSERT_DELTA(xyz[1], 2.0, (1e-10)); // Luminance is the same for constant spectrum.
      TS_ASSERT(xyz[2]>=0.0);
      }

    void test_SpectrumRoutines_SpectrumCoefToXYZ()
      {
      SpectrumCoef_d s(2.0,2.0,2.0);
      XYZColor_d xyz = SpectrumRoutines::SpectrumCoefToXYZ(s);
      TS_ASSERT(xyz[0]>=0.0);
      TS_ASSERT_DELTA(xyz[1], 2.0, (1e-10)); // Luminance is the same for constant spectrum.
      TS_ASSERT(xyz[2]>=0.0);
      }

    void test_SpectrumRoutines_XYZToSpectrum()
      {
      XYZColor_f xyz(1.0, 2.0, 3.0);
      Spectrum_f s = SpectrumRoutines::XYZToSpectrum(xyz);
      XYZColor_f xyz2 = SpectrumRoutines::SpectrumToXYZ(s);
      TS_ASSERT_DELTA(xyz[0], xyz2[0], (1e-5));
      TS_ASSERT_DELTA(xyz[1], xyz2[1], (1e-5));
      TS_ASSERT_DELTA(xyz[2], xyz2[2], (1e-5));
      }

    void test_SpectrumRoutines_XYZToSpectrumCoef()
      {
      XYZColor_f xyz(1.0, 2.0, 3.0);
      SpectrumCoef_f s = SpectrumRoutines::XYZToSpectrumCoef(xyz);
      XYZColor_f xyz2 = SpectrumRoutines::SpectrumCoefToXYZ(s);
      TS_ASSERT_DELTA(xyz[0], xyz2[0], (1e-5));
      TS_ASSERT_DELTA(xyz[1], xyz2[1], (1e-5));
      TS_ASSERT_DELTA(xyz[2], xyz2[2], (1e-5));
      }

    void test_SpectrumRoutines_Luminance1()
      {
      Spectrum_d s(3.0,2.0,1.0);
      XYZColor_d xyz = SpectrumRoutines::SpectrumToXYZ(s);
      TS_ASSERT_EQUALS(xyz[1], SpectrumRoutines::Luminance(s));
      }

    void test_SpectrumRoutines_Luminance2()
      {
      SpectrumCoef_d s(3.0,2.0,1.0);
      XYZColor_d xyz = SpectrumRoutines::SpectrumCoefToXYZ(s);
      TS_ASSERT_EQUALS(xyz[1], SpectrumRoutines::Luminance(s));
      }

    // Test with no samples
    void test_SpectrumRoutines_SampledToXYZ()
      {
      std::vector<std::pair<float,float>> samples;
      samples.push_back(std::make_pair(0.f,100.f));
      samples.push_back(std::make_pair(1000.f,200.f));
      XYZColor_f xyz = SpectrumRoutines::SampledToXYZ(samples);

      TS_ASSERT(xyz[0]>xyz[1]);
      TS_ASSERT(xyz[1]>xyz[2]);
      }

    // Test with no samples
    void test_SpectrumRoutines_SampledToSpectrum_NoSamples()
      {
      std::vector<std::pair<double,double>> samples;
      Spectrum_d s = SpectrumRoutines::SampledToSpectrum(samples);

      // Is no samples the spectrum is black.
      TS_ASSERT_EQUALS(s, Spectrum_d(0.0));
      }

    // Test with a single sample.
    void test_SpectrumRoutines_SampledToSpectrum_SingleSample()
      {
      std::vector<std::pair<double,double>> samples(1,std::make_pair(10.0,5.0));
      Spectrum_d s = SpectrumRoutines::SampledToSpectrum(samples);

      CustomAssertDelta(s, Spectrum_d(5.0), (1e-10));
      }

    // Test with two samples.
    void test_SpectrumRoutines_SampledToSpectrum_TwoSamples()
      {
      std::vector<std::pair<double,double>> samples;
      samples.push_back(std::make_pair(0,100));
      samples.push_back(std::make_pair(1000,200));
      Spectrum_d s = SpectrumRoutines::SampledToSpectrum(samples);
      XYZColor_d xyz = SpectrumRoutines::SampledToXYZ(samples);

      CustomAssertDelta(s, SpectrumRoutines::XYZToSpectrum(xyz), (1e-10));
      }

    void test_SpectrumRoutines_SampledToSpectrumCoef()
      {
      std::vector<std::pair<double,double>> samples;
      samples.push_back(std::make_pair(300,100));
      samples.push_back(std::make_pair(400,300));
      samples.push_back(std::make_pair(500,150));
      samples.push_back(std::make_pair(600,500));
      samples.push_back(std::make_pair(700,50));

      Spectrum_d s = SpectrumRoutines::SampledToSpectrum(samples);
      SpectrumCoef_d s2 = SpectrumRoutines::SampledToSpectrumCoef(samples);

      TS_ASSERT_EQUALS(s[0], s2[0]);
      TS_ASSERT_EQUALS(s[1], s2[1]);
      TS_ASSERT_EQUALS(s[2], s2[2]);
      }

  };

#endif // SPECTRUM_ROUTINES_TEST_H