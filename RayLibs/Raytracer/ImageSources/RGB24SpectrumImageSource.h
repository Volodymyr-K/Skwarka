#ifndef RGB24_SPECTRUM_IMAGE_SOURCE_H
#define RGB24_SPECTRUM_IMAGE_SOURCE_H

#include <Common/Common.h>
#include <Raytracer/Core/ImageSource.h>

/**
* Represents RGB color. Each color component is defined by unsigned char (one byte).
*/
struct RGB24
  {
  unsigned char m_rgb[3];
  };

/**
* ImageSource implementation that converts RGB image (with unsigned char defining each color component) to Spectrum image.
* The template parameter is the type of the Spectrum.
*/
template <typename T>
class RGB24SpectrumImageSource: public ImageSource<Spectrum<T> >
  {
  public:
    /**
    * Creates RGB24SpectrumImageSource with the specified RGB image and the scale factor.
    * The image values will be multiplied by the scale factor during conversion to Spectrum type.
    */
    RGB24SpectrumImageSource(const std::vector<std::vector<RGB24> > &i_values, double i_scale = 1.0);

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
    * Serializes RGB24SpectrumImageSource to/from the specified Archive. This method is used by the boost serialization framework.
    */
    template<class Archive>
    void serialize(Archive &i_ar, const unsigned int i_version);

  private:
    std::vector<std::vector<RGB24> > m_values;
    double m_scale;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
RGB24SpectrumImageSource<T>::RGB24SpectrumImageSource(const std::vector<std::vector<RGB24> > &i_values, double i_scale): m_values(i_values), m_scale(i_scale)
  {
  // Assert that all inner vectors are of the same size.
  for(size_t i=1;i<m_values.size();++i)
    ASSERT(m_values[i].size()==m_values[0].size());
  }

template <typename T>
void RGB24SpectrumImageSource<T>::GetImage(std::vector<std::vector<Spectrum<T> > > &o_image) const
  {
  o_image.resize(m_values.size());

  for(size_t i=0;i<m_values.size();++i)
    {
    const std::vector<RGB24> &source_row = m_values[i];

    std::vector<Spectrum<T> > &dest_row = o_image[i];
    dest_row.resize(m_values[i].size());

    // Just copy the values and multiply them by the scale factor.
    for(size_t j=0;j<m_values[i].size();++j)
      dest_row[j] = Spectrum<T>(source_row[j].m_rgb[0], source_row[j].m_rgb[1], source_row[j].m_rgb[2]) * m_scale;
    }
  }

template <typename T>
size_t RGB24SpectrumImageSource<T>::GetHeight() const
  {
  return m_values.size();
  }

template <typename T>
size_t RGB24SpectrumImageSource<T>::GetWidth() const
  {
  return m_values.empty() ? 0 : m_values[0].size();
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

/**
* Constructs RGB24SpectrumImageSource with the data from the specified Archive. This method is used by the boost serialization framework.
*/
template<typename T, class Archive>
void load_construct_data(Archive &i_ar, RGB24SpectrumImageSource<T> *ip_image_source, const unsigned int i_version)
  {
  // Construct with some dummy data, it will be serialized later in serialize() method.
  std::vector<std::vector<RGB24> > image(1, std::vector<RGB24>(1));
  ::new(ip_image_source)RGB24SpectrumImageSource<T>(image, 1.0);
  }

/**
* Serializes RGB24SpectrumImageSource to/from the specified Archive. This method is used by the boost serialization framework.
*/
template <typename T>
template<class Archive>
void RGB24SpectrumImageSource<T>::serialize(Archive &i_ar, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<ImageSource<Spectrum<T> > >(*this);
  i_ar & m_values;
  i_ar & m_scale;
  }

// The following code exports different specializations of the RGB24SpectrumImageSource template in the boost serialization framework.
// If you need to serialize a new specialization you have to add it here.
typedef RGB24SpectrumImageSource<float> RGB24SpectrumImageSource_float;
typedef RGB24SpectrumImageSource<double> RGB24SpectrumImageSource_double;

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(RGB24SpectrumImageSource_float)
BOOST_CLASS_EXPORT(RGB24SpectrumImageSource_double)

#endif // RGB24_SPECTRUM_IMAGE_SOURCE_H