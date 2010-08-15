#ifndef SPECTRUM_ROUTINES_H
#define SPECTRUM_ROUTINES_H

#include <Common/Common.h>
#include "Spectrum.h"
#include "Color.h"
#include <Math/MathRoutines.h>
#include <Math/Matrix3x3.h>
#include <vector>
#include <utility>
#include <algorithm>

namespace SpectrumRoutines
  {

  /**
  * Converts Spectrum to XYZ response values.
  */
  template<typename T>
  XYZColor<T> SpectrumToXYZ(const Spectrum<T> &i_spectrum);

  /**
  * Converts SpectrumCoef to XYZ response values.
  */
  template<typename T>
  XYZColor<T> SpectrumCoefToXYZ(const SpectrumCoef<T> &i_spectrum_coef);

  /**
  * Converts XYZ response values to Spectrum.
  * The conversion is unambiguous because only three samples are used to represent the spectrum.
  */
  template<typename T>
  Spectrum<T> XYZToSpectrum(const XYZColor<T> &i_xyz);

  /**
  * Converts XYZ response values to SpectrumCoef.
  * The conversion is unambiguous because only three samples are used to represent the spectrum.
  */
  template<typename T>
  SpectrumCoef<T> XYZToSpectrumCoef(const XYZColor<T> &i_xyz);

  /**
  * Returns luminance for the given Spectrum (i.e. Y response value).
  */
  template<typename T>
  T Luminance(const Spectrum<T> &i_spectrum);

  /**
  * Returns luminance for the given SpectrumCoef (i.e. Y response value).
  */
  template<typename T>
  T Luminance(const SpectrumCoef<T> &i_spectrum_coef);

  /**
  * Converts the sampled spectrum values to XYZ response values.
  * @param i_samples Vector with spectrum samples. Each vector element is the (wavelength, value) pair.
  * @return XYZ response values.
  */
  template<typename T>
  XYZColor<T> SampledToXYZ(const std::vector<std::pair<T,T> > &i_samples);

  /**
  * Converts the sampled spectrum values to Spectrum.
  * The conversion is done by first transforming the sampled spectrum to XYZ values and then XYZ values are converted to Spectrum.
  * @param i_samples Vector with spectrum samples. Each vector element is the (wavelength, value) pair.
  * @return Spectrum.
  */
  template<typename T>
  Spectrum<T> SampledToSpectrum(const std::vector<std::pair<T,T> > &i_samples);

  /**
  * Converts the sampled spectrum values to SpectrumCoef.
  * The conversion is done by first transforming the sampled spectrum to XYZ values and then XYZ values are converted to SpectrumCoef.
  * @param i_samples Vector with spectrum samples. Each vector element is the (wavelength, value) pair.
  * @return SpectrumCoef.
  */
  template<typename T>
  SpectrumCoef<T> SampledToSpectrumCoef(const std::vector<std::pair<T,T> > &i_samples);

  /////////////////////////////////////////////////////////////////////////////////////////////////////////

  /*
  * Defines (inclusive) starts and ends of the piecewise-constant regions of the spectrum representation used by Spectrum and SpectrumCoef classes.
  * The regions are chosen carefully to minimize the expected error of using only three samples instead of full spectrum.
  * The wavelengths correspond to the points where the X, Y and Z response curves intersect.
  */
  const int SPECTRUM_WAVELENGTH_START[3] = {360,495,575};
  const int SPECTRUM_WAVELENGTH_END[3] = {494,574,830};

  /*
  * CIE X, Y and Z response curves.
  */
  const int CIE_WAVELENGTH_START = 360;
  const int CIE_WAVELENGTH_END = 830;
  const int CIE_SAMPLES_NUM = CIE_WAVELENGTH_END-CIE_WAVELENGTH_START+1;
  extern const double CIE_X[CIE_SAMPLES_NUM];
  extern const double CIE_Y[CIE_SAMPLES_NUM];
  extern const double CIE_Z[CIE_SAMPLES_NUM];
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace SpectrumRoutines
  {

  /**
  * Helper class that is used to convert from Spectrum to XYZ and vice versa.
  * The class uses values of SPECTRUM_WAVELENGTH_START, SPECTRUM_WAVELENGTH_END arrays and CIE XYZ curves to initialize the transform matrices.
  * There's a global instance of the class that is initialized at the program's start-up and used by the spectrum conversion routines.
  */
  class XYZSpectrumConverter
    {
    public:
      /**
      * Creates XYZSpectrumConverter instance and initialize the transform matrices based on the values
      * of SPECTRUM_WAVELENGTH_START, SPECTRUM_WAVELENGTH_END arrays and CIE XYZ curves.
      */
      XYZSpectrumConverter();

      void SpectrumToXYZ(const double i_spectrum[3], double o_xyz[3]) const;

      void XYZToSpectrum(const double i_xyz[3], double o_spectrum[3]) const;

      double Luminance(const double i_spectrum[3]) const;

    private:
      Matrix3x3_d m_Spectrum_to_XYZ, m_XYZ_to_Spectrum;
    };

  /**
  * The global instance of the XYZSpectrumConverter class which is used by the spectrum routines below.
  */
  extern XYZSpectrumConverter global_xyz_spectrum_converter;

  template<typename T>
  XYZColor<T> SpectrumToXYZ(const Spectrum<T> &i_spectrum)
    {
    double xyz[3];
    double spectrum[3]={i_spectrum[0], i_spectrum[1], i_spectrum[2]};
    global_xyz_spectrum_converter.SpectrumToXYZ(spectrum, xyz);

    return XYZColor<T>((T)xyz[0], (T)xyz[1], (T)xyz[2]);
    }

  template<typename T>
  XYZColor<T> SpectrumCoefToXYZ(const SpectrumCoef<T> &i_spectrum_coef)
    {
    double xyz[3];
    double spectrum[3]={i_spectrum_coef[0], i_spectrum_coef[1], i_spectrum_coef[2]};
    global_xyz_spectrum_converter.SpectrumToXYZ(spectrum, xyz);

    return XYZColor<T>((T)xyz[0], (T)xyz[1], (T)xyz[2]);
    }

  template<typename T>
  Spectrum<T> XYZToSpectrum(const XYZColor<T> &i_xyz)
    {
    double xyz[3]={i_xyz[0],i_xyz[1],i_xyz[2]};
    double spectrum[3];
    global_xyz_spectrum_converter.XYZToSpectrum(xyz, spectrum);

    return Spectrum<T>((T)spectrum[0], (T)spectrum[1], (T)spectrum[2]);
    }

  template<typename T>
  SpectrumCoef<T> XYZToSpectrumCoef(const XYZColor<T> &i_xyz)
    {
    double xyz[3]={i_xyz[0],i_xyz[1],i_xyz[2]};
    double spectrum[3];
    global_xyz_spectrum_converter.XYZToSpectrum(xyz, spectrum);

    return SpectrumCoef<T>((T)spectrum[0], (T)spectrum[1], (T)spectrum[2]);
    }

  template<typename T>
  T Luminance(const Spectrum<T> &i_spectrum)
    {
    double spectrum[3]={i_spectrum[0], i_spectrum[1], i_spectrum[2]};
    return (T)global_xyz_spectrum_converter.Luminance(spectrum);
    }

  template<typename T>
  T Luminance(const SpectrumCoef<T> &i_spectrum_coef)
    {
    double spectrum[3]={i_spectrum_coef[0], i_spectrum_coef[1], i_spectrum_coef[2]};
    return (T)global_xyz_spectrum_converter.Luminance(spectrum);
    }

  template<typename T>
  double _InterpolateSortedSamples(const std::vector<std::pair<T,T> > &i_samples, double i_wavelength)
    {
    size_t n = i_samples.size();
    ASSERT(n>0);

    if (i_wavelength <= i_samples[0].first)   return i_samples[0].second;
    if (i_wavelength >= i_samples[n-1].first) return i_samples[n-1].second;

    size_t begin=0, end=n-2;
    while(begin<end)
      {
      ASSERT(i_samples[begin].first<=i_samples[end].first && i_wavelength>=i_samples[begin].first && i_wavelength<i_samples[end+1].first);
      int medium = (begin+end+1)/2;

      if (i_samples[medium].first <= i_wavelength)
        begin=medium;
      else
        end=medium-1;
      }

    double t = (i_wavelength-i_samples[begin].first)/(i_samples[begin+1].first-i_samples[begin].first);
    return MathRoutines::LinearInterpolate(t, (double)i_samples[begin].second, (double)i_samples[begin+1].second);
    }

  template<typename T>
  XYZColor<T> SampledToXYZ(const std::vector<std::pair<T,T> > &i_samples)
    {
    size_t n = i_samples.size();
    if (n==0)
      return XYZColor<T>();

    bool sorted=true;
    for(size_t i=1;i<n && sorted;++i)
      if (i_samples[i] < i_samples[i-1])
        sorted=false;

    // Use double values for calculation, convert to T later.
    double Y_sum = 0.0;
    XYZColor_d xyz;
    if (sorted)
      {
      for(size_t i=0;i<CIE_SAMPLES_NUM;++i)
        {
        double val = _InterpolateSortedSamples(i_samples, CIE_WAVELENGTH_START+i);
        xyz[0] += val * CIE_X[i];
        xyz[1] += val * CIE_Y[i];
        xyz[2] += val * CIE_Z[i];
        Y_sum += CIE_Y[i];
        }
      }
    else
      {
      std::vector<std::pair<T,T> > sorted_samples(i_samples.begin(), i_samples.end());
      std::sort(sorted_samples.begin(), sorted_samples.end());

      for(size_t i=0;i<CIE_SAMPLES_NUM;++i)
        {
        double val = _InterpolateSortedSamples(sorted_samples, CIE_WAVELENGTH_START+i);
        xyz[0] += val * CIE_X[i];
        xyz[1] += val * CIE_Y[i];
        xyz[2] += val * CIE_Z[i];
        Y_sum += CIE_Y[i];
        }
      }

    double inv_Y_sum = 1.0/Y_sum;
    xyz[0] *= inv_Y_sum;
    xyz[1] *= inv_Y_sum;
    xyz[2] *= inv_Y_sum;
    return XYZColor<T>((T)xyz[0], (T)xyz[1], (T)xyz[2]);
    }

  template<typename T>
  Spectrum<T> SampledToSpectrum(const std::vector<std::pair<T,T> > &i_samples)
    {
    return XYZToSpectrum(SampledToXYZ(i_samples));
    }

  template<typename T>
  SpectrumCoef<T> SampledToSpectrumCoef(const std::vector<std::pair<T,T> > &i_samples)
    {
    return XYZToSpectrumCoef(SampledToXYZ(i_samples));
    }

  }

#endif // SPECTRUM_ROUTINES_H