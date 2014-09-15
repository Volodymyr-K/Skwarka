#ifndef SCALE_TEXTURE_H
#define SCALE_TEXTURE_H

#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/Core/Texture.h>
#include <Raytracer/Core/DifferentialGeometry.h>

/**
* The texture implementation that multiplies two other pluggable textures.
* The template parameters T1 and T2 are the value types of the two pluggable textures. The multiplication of objects of these types should be an object of type T2.
*/
template<typename T1, typename T2>
class ScaleTexture: public Texture<T2>
  {
  public:
    /**
    * Crates ScaleTexture with the given textures.
    */
    ScaleTexture(intrusive_ptr<const Texture<T1> > ip_texture1, intrusive_ptr<const Texture<T2> > ip_texture2);

    /**
    * Returns the value corresponding to the specified DifferentialGeometry and triangle index.
    */
    virtual T2 Evaluate(const DifferentialGeometry &i_dg, size_t i_triangle_index) const;

  private:
    ScaleTexture() {}; // Empty default constructor for the boost serialization framework.

    // Needed for the boost serialization framework.  
    friend class boost::serialization::access;

    /**
    * Serializes to/from the specified Archive. This method is used by the boost serialization framework.
    */
    template<class Archive>
    void serialize(Archive &i_ar, const unsigned int i_version);

  private:
    intrusive_ptr<const Texture<T1> > mp_texture1;
    intrusive_ptr<const Texture<T2> > mp_texture2;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T1, typename T2>
ScaleTexture<T1,T2>::ScaleTexture(intrusive_ptr<const Texture<T1> > ip_texture1, intrusive_ptr<const Texture<T2> > ip_texture2):
Texture(), mp_texture1(ip_texture1), mp_texture2(ip_texture2)
  {
  ASSERT(ip_texture1);
  ASSERT(ip_texture2);
  }

template<typename T1, typename T2>
T2 ScaleTexture<T1,T2>::Evaluate(const DifferentialGeometry &i_dg, size_t i_triangle_index) const
  {
  return static_cast<T2>( mp_texture1->Evaluate(i_dg, i_triangle_index) * mp_texture2->Evaluate(i_dg, i_triangle_index) );
  }

template<typename T1, typename T2>
template<class Archive>
void ScaleTexture<T1,T2>::serialize(Archive &i_ar, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<Texture<T2> >(*this);
  i_ar & mp_texture1;
  i_ar & mp_texture2;
  }

// The following code exports different specializations of the ScaleTexture template in the boost serialization framework.
// If you need to serialize a new specialization you have to add it here.
typedef ScaleTexture<SpectrumCoef_f,Spectrum_f> ScaleTexture_SpectrumCoef_f_Spectrum_f;
typedef ScaleTexture<SpectrumCoef_d,Spectrum_d> ScaleTexture_SpectrumCoef_d_Spectrum_d;
typedef ScaleTexture<SpectrumCoef_f,SpectrumCoef_f> ScaleTexture_SpectrumCoef_f_SpectrumCoef_f;
typedef ScaleTexture<SpectrumCoef_d,SpectrumCoef_d> ScaleTexture_SpectrumCoef_d_SpectrumCoef_d;
typedef ScaleTexture<float,float> ScaleTexture_float_float;
typedef ScaleTexture<double,double> ScaleTexture_double_double;

BOOST_CLASS_EXPORT_KEY(ScaleTexture_SpectrumCoef_f_Spectrum_f)
BOOST_CLASS_EXPORT_KEY(ScaleTexture_SpectrumCoef_d_Spectrum_d)
BOOST_CLASS_EXPORT_KEY(ScaleTexture_SpectrumCoef_f_SpectrumCoef_f)
BOOST_CLASS_EXPORT_KEY(ScaleTexture_SpectrumCoef_d_SpectrumCoef_d)
BOOST_CLASS_EXPORT_KEY(ScaleTexture_float_float)
BOOST_CLASS_EXPORT_KEY(ScaleTexture_double_double)

#endif // SCALE_TEXTURE_H