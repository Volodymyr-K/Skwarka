#ifndef RGB24_SPECTRUM_IMAGE_SOURCE_H
#define RGB24_SPECTRUM_IMAGE_SOURCE_H

#include <Common/Common.h>
#include <Raytracer/Core/ImageSource.h>
#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/Core/SpectrumRoutines.h>
#include <Raytracer/Core/Color.h>
#include <cmath>

/**
* Represents RGB color. Each color component is defined by unsigned char (one byte).
*/
struct RGB24
  {
  unsigned char m_rgb[3];
  };

/**
* ImageSource implementation that converts RGB image (with unsigned char defining each color component) to 2D array of the specified type (see template parameter of the class).
* The class is also responsible for de-gamma correction of the input RGB values and converting the RGB values from the specified RGB color space.
* The template parameter is the target type of the conversion. The following target types are supported: Spectrum_f, Spectrum_d, SpectrumCoef_f, SpectrumCoef_d.
*/
template<typename T>
class RGB24ImageSource: public ImageSource<T>
  {
  public:
    /**
    * Creates RGB24ImageSource with the specified RGB image, RGB color space and the scale factor.
    * The resulting image values will be multiplied by the scale factor during conversion to the target type.
    */
    RGB24ImageSource(const std::vector<std::vector<RGB24> > &i_values, const ColorSystem &i_color_system, double i_scale = 1.0);

    /**
    * Gets 2D array of values (image) of the target type.
    */
    void GetImage(std::vector<std::vector<T> > &o_image) const;

    /**
    * Returns height of the image, i.e. the size of the outer vector which defines the image.
    */
    size_t GetHeight() const;

    /**
    * Returns width of the image, i.e. the size of the inner vector which defines the image.
    */
    size_t GetWidth() const;

  private:
    /**
    * Helper private method that converts XYZColor to the target type.
    * Specializations for Spectrum_f, Spectrum_d, SpectrumCoef_f and SpectrumCoef_d are provided.
    * The generic template implementation is not provided and thus the class won't compile with other target types.
    */
    T _XYZ_To_T(const XYZColor_d &i_color) const;

  private:
    RGB24ImageSource() {}; // Empty default constructor for the boost serialization framework.

    // Needed for the boost serialization framework.  
    friend class boost::serialization::access;

    /**
    * Serializes to/from the specified Archive. This method is used by the boost serialization framework.
    */
    template<class Archive>
    void serialize(Archive &i_ar, const unsigned int i_version);

  private:
    std::vector<std::vector<RGB24> > m_values;

    ColorSystem m_color_system;

    double m_scale;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
RGB24ImageSource<T>::RGB24ImageSource(const std::vector<std::vector<RGB24> > &i_values, const ColorSystem &i_color_system, double i_scale = 1.0):
m_values(i_values), m_color_system(i_color_system), m_scale(i_scale)
  {
  // Assert that all inner vectors are of the same size.
  for(size_t i=1;i<m_values.size();++i)
    ASSERT(m_values[i].size()==m_values[0].size());
  }

template<typename T>
void RGB24ImageSource<T>::GetImage(std::vector<std::vector<T> > &o_image) const
  {
  o_image.resize(m_values.size());

  for(size_t i=0;i<m_values.size();++i)
    {
    const std::vector<RGB24> &source_row = m_values[i];

    std::vector<T> &dest_row = o_image[i];
    dest_row.resize(m_values[i].size());

    // Just copy the values and multiply them by the scale factor.
    for(size_t j=0;j<m_values[i].size();++j)
      {
      RGBColor_d rgb_decoded = m_color_system.GammaDecode(RGBColor_d(
        source_row[j].m_rgb[0]/255.0,
        source_row[j].m_rgb[1]/255.0,
        source_row[j].m_rgb[2]/255.0));

      T ret = _XYZ_To_T(m_color_system.RGB_To_XYZ(rgb_decoded));
      dest_row[j] = ret * m_scale;
      }
    }
  }

template<typename T>
size_t RGB24ImageSource<T>::GetHeight() const
  {
  return m_values.size();
  }

template<typename T>
size_t RGB24ImageSource<T>::GetWidth() const
  {
  return m_values.empty() ? 0 : m_values[0].size();
  }

template<>
inline Spectrum_f RGB24ImageSource<Spectrum_f>::_XYZ_To_T(const XYZColor_d &i_color) const
  {
  return Convert<float>(SpectrumRoutines::XYZToSpectrum(i_color));
  }

template<>
inline Spectrum_d RGB24ImageSource<Spectrum_d>::_XYZ_To_T(const XYZColor_d &i_color) const
  {
  return SpectrumRoutines::XYZToSpectrum(i_color);
  }

template<>
inline SpectrumCoef_f RGB24ImageSource<SpectrumCoef_f>::_XYZ_To_T(const XYZColor_d &i_color) const
  {
  return Convert<float>(SpectrumRoutines::XYZToSpectrumCoef(i_color));
  }

template<>
inline SpectrumCoef_d RGB24ImageSource<SpectrumCoef_d>::_XYZ_To_T(const XYZColor_d &i_color) const
  {
  return SpectrumRoutines::XYZToSpectrumCoef(i_color);
  }

/**
* Serializes RGB24 to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void serialize(Archive &i_ar, RGB24 &i_rgb24, const unsigned int i_version)
  {
  i_ar & i_rgb24.m_rgb;
  }

// Don't store class info for RGB24.
BOOST_CLASS_IMPLEMENTATION(RGB24, boost::serialization::object_serializable)

template<typename T>
template<class Archive>
void RGB24ImageSource<T>::serialize(Archive &i_ar, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<ImageSource<T> >(*this);
  i_ar & m_values;
  i_ar & m_scale;
  i_ar & m_color_system;
  }

// The following code exports different specializations of the RGB24ImageSource template in the boost serialization framework.
// If you need to serialize a new specialization you have to add it here.
typedef RGB24ImageSource<Spectrum_f> RGB24ImageSource_Spectrum_float;
typedef RGB24ImageSource<Spectrum_d> RGB24ImageSource_Spectrum_double;

typedef RGB24ImageSource<SpectrumCoef_f> RGB24ImageSource_SpectrumCoef_float;
typedef RGB24ImageSource<SpectrumCoef_d> RGB24ImageSource_SpectrumCoef_double;

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(RGB24ImageSource_Spectrum_float)
BOOST_CLASS_EXPORT(RGB24ImageSource_Spectrum_double)
BOOST_CLASS_EXPORT(RGB24ImageSource_SpectrumCoef_float)
BOOST_CLASS_EXPORT(RGB24ImageSource_SpectrumCoef_double)

#endif // RGB24_SPECTRUM_IMAGE_SOURCE_H