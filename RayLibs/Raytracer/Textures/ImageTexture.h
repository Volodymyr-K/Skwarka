#ifndef IMAGE_TEXTURE_H
#define IMAGE_TEXTURE_H

#include <Common/Common.h>
#include <Raytracer/Core/Texture.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/Mapping.h>
#include <Raytracer/Core/MIPMap.h>
#include <Raytracer/Core/Spectrum.h>
#include <vector>

/**
* Texture implementation that looks up texture values in a 2D map (i.e. image).
* The texture uses pluggable Mapping2D implementation to map DifferentialGeometry to a 2D image point.
* The texture uses MIPMap to store the image and retrieve the values. The texture values are filtered with an anisotropic filter.
*
* The MemoryType template parameter corresponds to the type of the actual values stored by the texture.
* The ReturnType template parameter corresponds to the type of the values the texture returns.
* The Converter template parameter is a class used to convert from MemoryType to ReturnType. The default implementation is used if not specified otherwise.
*/
template<typename MemoryType, typename ReturnType, typename Converter = DefaultConverter<MemoryType,ReturnType>>
class ImageTexture: public Texture<ReturnType>
  {
  public:
    /**
    * Crates ImageTexture for the given image and 2D mapping.
    * @param i_image 2D array of the image values. All inner vectors should have the same size. Should have at least one row and at least one column.
    * @param ip_mapping 2D mapping used to map DifferentialGeometry to an image point. Should not be NULL.
    * @param i_repeat Sets whether to wrap the texture on its edges. If false, the value is considered zero (black) beyond the image bounds.
    * @param i_max_anisotropy Maximum anisotropy allowed (ratio of the major ellipse axis to its minor axis). Should be greater or equal than 1.0.
    */
    ImageTexture(const std::vector<std::vector<MemoryType>> &i_image, intrusive_ptr<const Mapping2D> ip_mapping, bool i_repeat = true, double i_max_anisotropy = 8.0);

    /**
    * Crates ImageTexture from the specified image source and 2D mapping.
    * @param ip_image_source ImageSource implementation that defines image for the ImageTexture. The image defined by the ImageSource should not be empty.
    * @param ip_mapping 2D mapping used to map DifferentialGeometry to an image point. Should not be NULL.
    * @param i_repeat Sets whether to wrap the texture on its edges. If false, the value is considered zero (black) beyond the image bounds.
    * @param i_max_anisotropy Maximum anisotropy allowed (ratio of the major ellipse axis to its minor axis). Should be greater or equal than 1.0.
    */
    ImageTexture(intrusive_ptr<const ImageSource<MemoryType>> ip_image_source, intrusive_ptr<const Mapping2D> ip_mapping, bool i_repeat = true, double i_max_anisotropy = 8.0);

    /**
    * Crates ImageTexture from the specified MIPMap and 2D mapping.
    * @param ip_mip_map MIPMap object that defines image for the ImageTexture.
    * @param ip_mapping 2D mapping used to map DifferentialGeometry to an image point. Should not be NULL.
    */
    ImageTexture(intrusive_ptr<const MIPMap<MemoryType>> ip_mip_map, intrusive_ptr<const Mapping2D> ip_mapping);

    /**
    * Returns the value corresponding to the specified DifferentialGeometry and triangle index.
    */
    ReturnType Evaluate(const DifferentialGeometry &i_dg, size_t i_triangle_index) const;

  private:
    ImageTexture() {}; // Empty default constructor for the boost serialization framework.

    // Needed for the boost serialization framework.  
    friend class boost::serialization::access;

    /**
    * Serializes to/from the specified Archive. This method is used by the boost serialization framework.
    */
    template<class Archive>
    void serialize(Archive &i_ar, const unsigned int i_version);

  private:
    intrusive_ptr<const MIPMap<MemoryType>> mp_mip_map;
    intrusive_ptr<const Mapping2D> mp_mapping;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename MemoryType, typename ReturnType, typename Converter>
ImageTexture<MemoryType,ReturnType,Converter>::ImageTexture(const std::vector<std::vector<MemoryType>> &i_image,
                                                  intrusive_ptr<const Mapping2D> ip_mapping, bool i_repeat, double i_max_anisotropy): mp_mapping(ip_mapping)
  {
  ASSERT(ip_mapping);

  mp_mip_map.reset(new MIPMap<MemoryType>(i_image, i_repeat, i_max_anisotropy) );
  }

template<typename MemoryType, typename ReturnType, typename Converter>
ImageTexture<MemoryType,ReturnType,Converter>::ImageTexture(intrusive_ptr<const ImageSource<MemoryType>> ip_image_source,
                                                            intrusive_ptr<const Mapping2D> ip_mapping, bool i_repeat, double i_max_anisotropy): mp_mapping(ip_mapping)
  {
  ASSERT(ip_image_source);
  ASSERT(ip_image_source->GetHeight()>0 && ip_image_source->GetWidth()>0);
  ASSERT(ip_mapping);

  mp_mip_map.reset(new MIPMap<MemoryType>(ip_image_source, i_repeat, i_max_anisotropy) );
  }

template<typename MemoryType, typename ReturnType, typename Converter>
ImageTexture<MemoryType, ReturnType, Converter>::ImageTexture(intrusive_ptr<const MIPMap<MemoryType>> ip_mip_map, intrusive_ptr<const Mapping2D> ip_mapping) :
mp_mapping(ip_mapping), mp_mip_map(ip_mip_map)
  {
  ASSERT(ip_mapping);
  ASSERT(ip_mip_map);
  }

template<typename MemoryType, typename ReturnType, typename Converter>
ReturnType ImageTexture<MemoryType,ReturnType,Converter>::Evaluate(const DifferentialGeometry &i_dg, size_t i_triangle_index) const
  {
  Point2D_d point;
  Vector2D_d dp_dx, dp_dy;
  mp_mapping->Map(i_dg, i_triangle_index, point, dp_dx, dp_dy);

  return Converter::Convert( mp_mip_map->Evaluate(point, dp_dx, dp_dy) );
  }

////////////////////////////////////////// DefaultConverter ///////////////////////////////////////////////

// The generic Converter implementation.
template<typename InputType, typename OutputType>
class DefaultConverter
  {
  public:
    static OutputType Convert(const InputType &i_input)
      {
      return static_cast<OutputType>(i_input);
      }
  };

// Explicit specialization for Spectrum type. Uses native Spectrum conversion routine.
template<typename InputSpectrumType, typename OutputSpectrumType>
class DefaultConverter<Spectrum<InputSpectrumType>, Spectrum<OutputSpectrumType>>
  {
  public:
    static Spectrum<OutputSpectrumType> Convert(const Spectrum<InputSpectrumType> &i_input)
      {
      return ::Convert<OutputSpectrumType>(i_input);
      }
  };

// Explicit specialization for SpectrumCoef type. Uses native SpectrumCoef conversion routine.
template<typename InputSpectrumType, typename OutputSpectrumType>
class DefaultConverter<SpectrumCoef<InputSpectrumType>, SpectrumCoef<OutputSpectrumType>>
  {
  public:
    static SpectrumCoef<OutputSpectrumType> Convert(const SpectrumCoef<InputSpectrumType> &i_input)
      {
      return ::Convert<OutputSpectrumType>(i_input);
      }
  };

/////////////////////////////////////////// Serialization ////////////////////////////////////////////////

template<typename MemoryType, typename ReturnType, typename Converter>
template<class Archive>
void ImageTexture<MemoryType,ReturnType,Converter>::serialize(Archive &i_ar, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<Texture<ReturnType>>(*this);

  i_ar & mp_mip_map;
  i_ar & mp_mapping;
  }

// The following code exports different specializations of the ImageTexture template in the boost serialization framework.
// If you need to serialize a new specialization you have to add it here.
typedef ImageTexture<Spectrum_f, Spectrum_f, DefaultConverter<Spectrum_f,Spectrum_f>> ImageTexture_Spectrum_f_Spectrum_f;
typedef ImageTexture<Spectrum_f, Spectrum_d, DefaultConverter<Spectrum_f,Spectrum_d>> ImageTexture_Spectrum_f_Spectrum_d;
typedef ImageTexture<Spectrum_d, Spectrum_d, DefaultConverter<Spectrum_d,Spectrum_d>> ImageTexture_Spectrum_d_Spectrum_d;

typedef ImageTexture<SpectrumCoef_f, SpectrumCoef_f, DefaultConverter<SpectrumCoef_f,SpectrumCoef_f>> ImageTexture_SpectrumCoef_f_SpectrumCoef_f;
typedef ImageTexture<SpectrumCoef_f, SpectrumCoef_d, DefaultConverter<SpectrumCoef_f,SpectrumCoef_d>> ImageTexture_SpectrumCoef_f_SpectrumCoef_d;
typedef ImageTexture<SpectrumCoef_d, SpectrumCoef_d, DefaultConverter<SpectrumCoef_d,SpectrumCoef_d>> ImageTexture_SpectrumCoef_d_SpectrumCoef_d;

typedef ImageTexture<float, float, DefaultConverter<float,float>> ImageTexture_float_float;
typedef ImageTexture<double, double, DefaultConverter<double,double>> ImageTexture_double_double;

typedef ImageTexture<float, double, DefaultConverter<float,double>> ImageTexture_float_double;

BOOST_CLASS_EXPORT_KEY(ImageTexture_Spectrum_f_Spectrum_f)
BOOST_CLASS_EXPORT_KEY(ImageTexture_Spectrum_f_Spectrum_d)
BOOST_CLASS_EXPORT_KEY(ImageTexture_Spectrum_d_Spectrum_d)
BOOST_CLASS_EXPORT_KEY(ImageTexture_SpectrumCoef_f_SpectrumCoef_f)
BOOST_CLASS_EXPORT_KEY(ImageTexture_SpectrumCoef_f_SpectrumCoef_d)
BOOST_CLASS_EXPORT_KEY(ImageTexture_SpectrumCoef_d_SpectrumCoef_d)
BOOST_CLASS_EXPORT_KEY(ImageTexture_float_float)
BOOST_CLASS_EXPORT_KEY(ImageTexture_double_double)
BOOST_CLASS_EXPORT_KEY(ImageTexture_float_double)

#endif // IMAGE_TEXTURE_H
