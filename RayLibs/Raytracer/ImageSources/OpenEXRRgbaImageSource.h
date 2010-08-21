#ifndef OPENEXR_RGBA_SPECTRUM_IMAGE_SOURCE_H
#define OPENEXR_RGBA_SPECTRUM_IMAGE_SOURCE_H

#include <Common/Common.h>
#include <Raytracer/Core/ImageSource.h>
#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/Core/SpectrumRoutines.h>
#include <Raytracer/Core/Color.h>
#include <ImfRgbaFile.h>
#include <ImfRgba.h>
#include <ImfArray.h>
#include <ImfChromaticitiesAttribute.h>
#include <string>
#include <exception>

/**
* ImageSource implementation that converts Open EXR Rgba image to 2D array of the specified type (see template parameter of the class).
* The class is also responsible for converting the RGB values from the specified RGB color space.
* Note that OpenEXR does not use gamma correction so there's no need to de-gamma the RGB values.
* In Rgba color from the Open EXR library each color component is defined by the "half" type which is two bytes long.
* The template parameter is the target type of the conversion. The following target types are supported: Spectrum_f, Spectrum_d, SpectrumCoef_f, SpectrumCoef_d.
*/
template <typename T>
class OpenEXRRgbaImageSource: public ImageSource<T>
  {
  public:
    /**
    * Creates OpenEXRRgbaImageSource with the specified Open EXR Rgba image, RGB color space and the scale factor.
    * The image values will be multiplied by the scale factor during conversion to the target type.
    * @param i_values Vector defining the 2D array in row-major order. Should have exactly i_width*i_height elements;
    * @param i_width Width of the input 2D array.
    * @param i_height Height of the input 2D array.
    * @param i_color_system RGB color system in which the colors in the input arrays are defined.
    * @param i_scale Scale factor for the resulting image values.
    */
    OpenEXRRgbaImageSource(const std::vector<Imf::Rgba> &i_values, size_t i_width, size_t i_height, const ColorSystem &i_color_system, double i_scale = 1.0);

    /**
    * Creates OpenEXRRgbaImageSource from the specified OpenEXR file and with the specified scale factor.
    * The RGB color space is read from the file's attributes. If it is not set in the attributes the default one is used (as per the OpenEXR's documentation it matches Rec. ITU-R BT.709-3)
    * The image values will be multiplied by the scale factor during conversion to the target type.
    */
    OpenEXRRgbaImageSource(std::string i_filename, double i_scale = 1.0);

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
    // Needed for the boost serialization framework.  
    friend class boost::serialization::access;

    /**   
    * Serializes OpenEXRRgbaImageSource to/from the specified Archive. This method is used by the boost serialization framework.
    */
    template<class Archive>
    void serialize(Archive &i_ar, const unsigned int i_version);

  private:
    std::vector<Imf::Rgba> m_values;
    size_t m_width, m_height;

    ColorSystem m_color_system;

    double m_scale;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
OpenEXRRgbaImageSource<T>::OpenEXRRgbaImageSource(const std::vector<Imf::Rgba> &i_values, size_t i_width, size_t i_height, const ColorSystem &i_color_system, double i_scale):
m_values(i_values), m_width(i_width), m_height(i_height), m_color_system(i_color_system), m_scale(i_scale)
  {
  ASSERT(m_values.size()==m_width*m_height);

  if (m_values.empty())
    m_width=m_height=0;
  }

template <typename T>
OpenEXRRgbaImageSource<T>::OpenEXRRgbaImageSource(std::string i_filename, double i_scale): m_scale(i_scale)
  {
  try
    {
    Imf::RgbaInputFile file(i_filename.c_str());
    Imath::Box2i dw = file.dataWindow();
    m_width  = dw.max.x - dw.min.x + 1;
    m_height = dw.max.y - dw.min.y + 1;
    m_values.resize(m_width*m_height);

    file.setFrameBuffer(&m_values[0] - dw.min.x - dw.min.y * m_width, 1, m_width);
    file.readPixels(dw.min.y, dw.max.y);

    const Imf::ChromaticitiesAttribute *p_chromaticities = file.header().findTypedAttribute<Imf::ChromaticitiesAttribute>("chromaticities");
    if (p_chromaticities)
      {
      const Imf::Chromaticities &chromaticities = p_chromaticities->value();
      Point2D_d red(chromaticities.red.x, chromaticities.red.y);
      Point2D_d green(chromaticities.green.x, chromaticities.green.y);
      Point2D_d blue(chromaticities.blue.x, chromaticities.blue.y);
      Point2D_d white(chromaticities.white.x, chromaticities.white.y);

      m_color_system = ColorSystem(red, green, blue, white, 1.0);
      }
    else
      {
      // Set the default color system (as per the OpenEXR's documentation it matches Rec. ITU-R BT.709-3).
      m_color_system = ColorSystem(Point2D_d(0.64,0.33), Point2D_d(0.30,0.60), Point2D_d(0.15,0.06), Point2D_d(0.3127,0.3290), 1.0);
      }
    }
  catch (const std::exception &)
    {
    m_width=m_height=0;
    m_values.clear();
    }
  }

template <typename T>
void OpenEXRRgbaImageSource<T>::GetImage(std::vector<std::vector<T> > &o_image) const
  {
  o_image.assign(m_height, std::vector<T>(m_width));

  // Just copy the values and multiply them by the scale factor.
  for(size_t i=0;i<m_height;++i)
    {
    std::vector<T> &dest_row = o_image[i];
    size_t offset = i*m_width;

    for(size_t j=0;j<m_width;++j)
      {
      RGBColor_d rgb(static_cast<float>(m_values[offset+j].r), static_cast<float>(m_values[offset+j].g), static_cast<float>(m_values[offset+j].b));

      T ret = _XYZ_To_T(m_color_system.RGB_To_XYZ(rgb));
      dest_row[j] = ret * m_scale;
      }
    }
  }

template <typename T>
size_t OpenEXRRgbaImageSource<T>::GetHeight() const
  {
  return m_height;
  }

template <typename T>
size_t OpenEXRRgbaImageSource<T>::GetWidth() const
  {
  return m_width;
  }

template<>
inline Spectrum_f OpenEXRRgbaImageSource<Spectrum_f>::_XYZ_To_T(const XYZColor_d &i_color) const
  {
  return Convert<float>(SpectrumRoutines::XYZToSpectrum(i_color));
  }

template<>
inline Spectrum_d OpenEXRRgbaImageSource<Spectrum_d>::_XYZ_To_T(const XYZColor_d &i_color) const
  {
  return SpectrumRoutines::XYZToSpectrum(i_color);
  }

template<>
inline SpectrumCoef_f OpenEXRRgbaImageSource<SpectrumCoef_f>::_XYZ_To_T(const XYZColor_d &i_color) const
  {
  return Convert<float>(SpectrumRoutines::XYZToSpectrumCoef(i_color));
  }

template<>
inline SpectrumCoef_d OpenEXRRgbaImageSource<SpectrumCoef_d>::_XYZ_To_T(const XYZColor_d &i_color) const
  {
  return SpectrumRoutines::XYZToSpectrumCoef(i_color);
  }

/**
* Constructs OpenEXRRgbaImageSource with the data from the specified Archive. This method is used by the boost serialization framework.
*/
template<typename T, class Archive>
void load_construct_data(Archive &i_ar, OpenEXRRgbaImageSource<T> *ip_image_source, const unsigned int i_version)
  {
  // Construct with some dummy data, it will be serialized later in serialize() method.
  std::vector<Imf::Rgba> image(1);
  ::new(ip_image_source)OpenEXRRgbaImageSource<T>(image, 1, 1, global_sRGB_E_ColorSystem, 1.0);
  }

/**
* Serializes OpenEXRRgbaImageSource to/from the specified Archive. This method is used by the boost serialization framework.
*/
template <typename T>
template<class Archive>
void OpenEXRRgbaImageSource<T>::serialize(Archive &i_ar, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<ImageSource<T> >(*this);
  i_ar & m_values;
  i_ar & m_width;
  i_ar & m_height;
  i_ar & m_color_system;
  i_ar & m_scale;
  }

// The following code exports different specializations of the OpenEXRRgbaImageSource template in the boost serialization framework.
// If you need to serialize a new specialization you have to add it here.
typedef OpenEXRRgbaImageSource<Spectrum_f> OpenEXRRgbaImageSource_Spectrum_float;
typedef OpenEXRRgbaImageSource<Spectrum_d> OpenEXRRgbaImageSource_Spectrum_double;

typedef OpenEXRRgbaImageSource<SpectrumCoef_f> OpenEXRRgbaImageSource_SpectrumCoef_float;
typedef OpenEXRRgbaImageSource<SpectrumCoef_d> OpenEXRRgbaImageSource_SpectrumCoef_double;


#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(OpenEXRRgbaImageSource_Spectrum_float)
BOOST_CLASS_EXPORT(OpenEXRRgbaImageSource_Spectrum_double)
BOOST_CLASS_EXPORT(OpenEXRRgbaImageSource_SpectrumCoef_float)
BOOST_CLASS_EXPORT(OpenEXRRgbaImageSource_SpectrumCoef_double)

/////////////////////////////////// Serialization for Open EXR classes ////////////////////////////////////////

/**
* Serializes Imf::Rgba to/from the specified Archive. This method is used by the boost serialization framework.
*/
namespace Imf
  {
  template<class Archive>
  void serialize(Archive &i_ar, Imf::Rgba &i_rgba, const unsigned int i_version)
    {
    i_ar & i_rgba.r;
    i_ar & i_rgba.g;
    i_ar & i_rgba.b;
    i_ar & i_rgba.a;
    }
  }

template<class Archive>
void save(Archive &i_ar, const half &i_half, const unsigned int i_version)
  {
  unsigned short bits = i_half.bits();
  i_ar << bits;
  }

template<class Archive>
void load(Archive &i_ar, half &i_half, const unsigned int i_version)
  {
  unsigned short bits;
  i_ar >> bits;
  i_half.setBits(bits);
  }

template<class Archive>
void serialize(Archive &i_ar, half &i_half, const unsigned int i_version)
  {
  boost::serialization::split_free(i_ar, i_half, i_version);
  }

// Don't store class info for Imf::Rgba.
BOOST_CLASS_IMPLEMENTATION(Imf::Rgba, boost::serialization::object_serializable)

// Don't store class info for half.
BOOST_CLASS_IMPLEMENTATION(half, boost::serialization::object_serializable)

#endif // OPENEXR_RGBA_SPECTRUM_IMAGE_SOURCE_H