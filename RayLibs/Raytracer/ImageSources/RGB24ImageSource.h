#ifndef RGB24_SPECTRUM_IMAGE_SOURCE_H
#define RGB24_SPECTRUM_IMAGE_SOURCE_H

#include <Common/Common.h>
#include <Raytracer/Core/ImageSource.h>
#include <Raytracer/Core/Spectrum.h>

/**
* Represents RGB color. Each color component is defined by unsigned char (one byte).
*/
struct RGB24
  {
  unsigned char m_rgb[3];
  };

/**
* ImageSource implementation that converts RGB image (with unsigned char defining each color component) to 2D array of the specified type (see template parameter of the class).
* The template parameter is the target type of the conversion. The type should support constructor with (unsigned char, unsigned char, unsigned char) parameters.
*/
template <typename T>
class RGB24ImageSource: public ImageSource<T>
  {
  public:
    /**
    * Creates RGB24ImageSource with the specified RGB image and the scale factor.
    * The image values will be multiplied by the scale factor during conversion to the target type.
    */
    RGB24ImageSource(const std::vector<std::vector<RGB24> > &i_values, double i_scale = 1.0);

    /**
    * Gets 2D array of values (image) of the target type.
    * The method should always return the same image for the same class instance.
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
    // Needed for the boost serialization framework.  
    friend class boost::serialization::access;

    /**
    * Serializes RGB24ImageSource to/from the specified Archive. This method is used by the boost serialization framework.
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
RGB24ImageSource<T>::RGB24ImageSource(const std::vector<std::vector<RGB24> > &i_values, double i_scale): m_values(i_values), m_scale(i_scale)
  {
  // Assert that all inner vectors are of the same size.
  for(size_t i=1;i<m_values.size();++i)
    ASSERT(m_values[i].size()==m_values[0].size());
  }

template <typename T>
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
      dest_row[j] = T(source_row[j].m_rgb[0], source_row[j].m_rgb[1], source_row[j].m_rgb[2]) * m_scale;
    }
  }

template <typename T>
size_t RGB24ImageSource<T>::GetHeight() const
  {
  return m_values.size();
  }

template <typename T>
size_t RGB24ImageSource<T>::GetWidth() const
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
* Constructs RGB24ImageSource with the data from the specified Archive. This method is used by the boost serialization framework.
*/
template<typename T, class Archive>
void load_construct_data(Archive &i_ar, RGB24ImageSource<T> *ip_image_source, const unsigned int i_version)
  {
  // Construct with some dummy data, it will be serialized later in serialize() method.
  std::vector<std::vector<RGB24> > image(1, std::vector<RGB24>(1));
  ::new(ip_image_source)RGB24ImageSource<T>(image, 1.0);
  }

/**
* Serializes RGB24ImageSource to/from the specified Archive. This method is used by the boost serialization framework.
*/
template <typename T>
template<class Archive>
void RGB24ImageSource<T>::serialize(Archive &i_ar, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<ImageSource<T> >(*this);
  i_ar & m_values;
  i_ar & m_scale;
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