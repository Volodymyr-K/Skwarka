#ifndef OPENEXR_RGBA_SPECTRUM_IMAGE_SOURCE_H
#define OPENEXR_RGBA_SPECTRUM_IMAGE_SOURCE_H

#include <Common/Common.h>
#include <Raytracer/Core/ImageSource.h>
#include <ImfRgba.h>
#include <ImfArray.h>

/**
* ImageSource implementation that converts Open EXR Rgba image to Spectrum image.
* In Rgba color from the Open EXR library each color component is defined by the "half" type which is two bytes long.
* The template parameter is the type of the Spectrum.
*/
template <typename T>
class OpenEXRRgbaSpectrumImageSource: public ImageSource<Spectrum<T> >
  {
  public:
    /**
    * Creates OpenEXRRgbaSpectrumImageSource with the specified Open EXR Rgba image and the scale factor.
    * The image values will be multiplied by the scale factor during conversion to Spectrum type.
    */
    OpenEXRRgbaSpectrumImageSource(const std::vector<std::vector<Imf::Rgba> > &i_values, double i_scale = 1.0);

    /**
    * Creates OpenEXRRgbaSpectrumImageSource with the specified Open EXR array of Rgba values and the scale factor.
    * The image values will be multiplied by the scale factor during conversion to Spectrum type.
    */
    OpenEXRRgbaSpectrumImageSource(const Imf::Array2D<Imf::Rgba> &i_values, size_t i_width, size_t i_height, double i_scale = 1.0);

    /**
    * Gets 2D array of values (image) of Spectrum type.
    * The method should always return the same image for the same class instance.
    */
    void GetImage(std::vector<std::vector<Spectrum<T> > > &o_image) const;

    /**
    * Returns height of the image, i.e. the size of the outer vector which defines the image.
    */
    size_t GetHeight() const;

    /**
    * Returns width of the image, i.e. the size of the inner vector which defines the image.
    */
    size_t GetWidth() const;

  private:
    // Needed for the boost serialization framework.  
    friend class boost::serialization::access;

    /**   
    * Serializes OpenEXRRgbaSpectrumImageSource to/from the specified Archive. This method is used by the boost serialization framework.
    */
    template<class Archive>
    void serialize(Archive &i_ar, const unsigned int i_version);

  private:
    std::vector<std::vector<Imf::Rgba> > m_values;
    double m_scale;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
OpenEXRRgbaSpectrumImageSource<T>::OpenEXRRgbaSpectrumImageSource(const std::vector<std::vector<Imf::Rgba> > &i_values, double i_scale): m_values(i_values), m_scale(i_scale)
  {
  for(size_t i=1;i<m_values.size();++i)
    ASSERT(m_values[i].size()==m_values[0].size());
  }

template <typename T>
OpenEXRRgbaSpectrumImageSource<T>::OpenEXRRgbaSpectrumImageSource(const Imf::Array2D<Imf::Rgba> &i_values, size_t i_width, size_t i_height, double i_scale):
m_values(i_height, std::vector<Imf::Rgba>(i_width)), m_scale(i_scale)
  {
  for(size_t i=0;i<i_height;++i)
    {
    std::vector<Imf::Rgba> &dest_row = m_values[i];

    for(size_t j=0;j<i_width;++j)
      dest_row[j] = i_values[(long)i][(long)j];
    }
  }

template <typename T>
void OpenEXRRgbaSpectrumImageSource<T>::GetImage(std::vector<std::vector<Spectrum<T> > > &o_image) const
  {
  o_image.resize(m_values.size());

  for(size_t i=0;i<m_values.size();++i)
    {
    const std::vector<Imf::Rgba> &source_row = m_values[i];

    std::vector<Spectrum<T> > &dest_row = o_image[i];
    dest_row.resize(m_values[i].size());

    // Just copy the values and multiply them by the scale factor.
    for(size_t j=0;j<m_values[i].size();++j)
      dest_row[j] = Spectrum<T>(static_cast<T>(source_row[j].r), static_cast<T>(source_row[j].g), static_cast<T>(source_row[j].b)) * m_scale;
    }
  }

template <typename T>
size_t OpenEXRRgbaSpectrumImageSource<T>::GetHeight() const
  {
  return m_values.size();
  }

template <typename T>
size_t OpenEXRRgbaSpectrumImageSource<T>::GetWidth() const
  {
  return m_values.empty() ? 0 : m_values[0].size();
  }

/**
* Constructs OpenEXRRgbaSpectrumImageSource with the data from the specified Archive. This method is used by the boost serialization framework.
*/
template<typename T, class Archive>
void load_construct_data(Archive &i_ar, OpenEXRRgbaSpectrumImageSource<T> *ip_image_source, const unsigned int i_version)
  {
  // Construct with some dummy data, it will be serialized later in serialize() method.
  std::vector<std::vector<Imf::Rgba> > image(1, std::vector<Imf::Rgba>(1));
  ::new(ip_image_source)OpenEXRRgbaSpectrumImageSource<T>(image, 1.0);
  }

/**
* Serializes OpenEXRRgbaSpectrumImageSource to/from the specified Archive. This method is used by the boost serialization framework.
*/
template <typename T>
template<class Archive>
void OpenEXRRgbaSpectrumImageSource<T>::serialize(Archive &i_ar, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<ImageSource<Spectrum<T> > >(*this);
  i_ar & m_values;
  i_ar & m_scale;
  }

// The following code exports different specializations of the OpenEXRRgbaSpectrumImageSource template in the boost serialization framework.
// If you need to serialize a new specialization you have to add it here.
typedef OpenEXRRgbaSpectrumImageSource<float> OpenEXRRgbaSpectrumImageSource_float;
typedef OpenEXRRgbaSpectrumImageSource<double> OpenEXRRgbaSpectrumImageSource_double;

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(OpenEXRRgbaSpectrumImageSource_float)
BOOST_CLASS_EXPORT(OpenEXRRgbaSpectrumImageSource_double)


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