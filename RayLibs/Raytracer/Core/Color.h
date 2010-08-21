#ifndef COLOR_H
#define COLOR_H

#include <Common/Common.h>
#include <Math/Matrix3x3.h>
#include <Math/Point2D.h>
#include <Math/MathRoutines.h>

/**
* Represents RGB color in some RGB color space.
* The template parameter corresponds to the type of the color values.
* @sa XYZColor, ColorSystem
*/
template<typename T>
struct RGBColor
  {
  RGBColor();
  RGBColor(T i_r, T i_g, T i_b);

  T operator[](unsigned char i_index) const;
  T &operator[](unsigned char i_index);

  /**
  * Clamps the color components to the specified range.
  */
  void Clamp(T i_low, T i_high);

  T m_rgb[3];
  };

/**
* Represents color in the CIE XYZ color space.
* The template parameter corresponds to the type of the color values.
* @sa RGBColor, ColorSystem
*/
template<typename T>
struct XYZColor
  {
  XYZColor();
  XYZColor(T i_x, T i_y, T i_z);

  T operator[](unsigned char i_index) const;
  T &operator[](unsigned char i_index);

  /**
  * Clamps the color components to the specified range.
  */
  void Clamp(T i_low, T i_high);

  T m_xyz[3];
  };

/**
* Returns true if all color components are in the specified range (inclusive).
*/
template<typename T>
bool InRange(const RGBColor<T> &i_color, T i_low, T i_high);

/**
* Returns true if all color components are in the specified range (inclusive).
*/
template<typename T>
bool InRange(const XYZColor<T> &i_color, T i_low, T i_high);

typedef RGBColor<float> RGBColor_f;
typedef RGBColor<double> RGBColor_d;

typedef XYZColor<float> XYZColor_f;
typedef XYZColor<double> XYZColor_d;

/**
* Represents color system defined by the CIE coordinates of its three primary colors and the coordinates of the white point.
* The class provides methods to convert between RGB and XYZ color spaces and methods for gamma corrections of RGB colors.
*/
class ColorSystem
  {
  public:
    /**
    * Creates color system that corresponds to the CIE XYZ space with gamma equal to 1.0.
    * This basically corresponds to the identity transformation between XYZ and RGB spaces.
    */
    ColorSystem();

    /**
    * Creates color system with the specified CIE coordinates of its three primary colors, coordinates of the white point and gamma value.
    */
    ColorSystem(Point2D_d i_red, Point2D_d i_green, Point2D_d i_blue, Point2D_d i_whitel, double i_gamma);

    Point2D_d GetRedPrimaryPoint() const;

    Point2D_d GetGreenPrimaryPoint() const;

    Point2D_d GetBluePrimaryPoint() const;

    Point2D_d GetWhitePoint() const;

    double GetGamma() const;

    /**
    * Converts XYZ color to the RGB color space and (optionally) constrain the converted point to lie within the boundaries of the
    * RGB triangle formed by the three primaries.
    * If the i_xyz_color is outside of the RGB triangle and i_constrain is false one of the RGB components will be negative.
    */
    template<typename T>
    RGBColor<T> XYZ_To_RGB(const XYZColor<T> &i_xyz_color, bool i_constrain) const;

    /**
    * Converts RGB color to the XYZ color space.
    */
    template<typename T>
    XYZColor<T> RGB_To_XYZ(const RGBColor<T> &i_rgb_color) const;

    /**
    * Gamma correction of the specified RGB color. Each color component is raised to the power of 1/gamma.
    * @param i_rgb_color RGB color to be gamma corrected. All color elements must be in [0;1] range.
    * @return Resulting gamma corrected color. All color elements will be in [0;1] range.
    */
    template<typename T>
    RGBColor<T> GammaEncode(const RGBColor<T> &i_rgb_color) const;

    /**
    * Inverse gamma correction of the specified RGB color. Each color component is raised to the power of gamma.
    * @param i_rgb_color RGB color to be gamma corrected. All color elements must be in [0;1] range.
    * @return Resulting de-gamma corrected color. All color elements will be in [0;1] range.
    */
    template<typename T>
    RGBColor<T> GammaDecode(const RGBColor<T> &i_rgb_color) const;

  private:
    double _Dot(const double i_a[3], const double i_b[3]) const;

  private:
    // Needed for the boost serialization framework.
    friend class boost::serialization::access;

    /**
    * Serializes ColorSystem to/from the specified Archive. This method is used by the boost serialization framework.
    */
    template<typename Archive>
    void serialize(Archive &i_ar, const unsigned int i_version);

  private:
    Point2D_d m_red, m_green, m_blue, m_white;
    Matrix3x3_d m_XYZ_To_RGB, m_RGB_To_XYZ;
    double m_gamma, m_inv_gamma;
  };

// Global instance of the ColorSystem that corresponds to the sRGB color space with D65 white point.
// This color system is suitable to convert to/from the sRGB system for illuminants (see Spectrum class).
extern const ColorSystem global_sRGB_D65_ColorSystem;

// Global instance of the ColorSystem that corresponds to the sRGB color space with E white point.
// This color system can be used to convert to/from the sRGB system for reflections (see SpectrumCoef class).
extern const ColorSystem global_sRGB_E_ColorSystem;

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////// RGBColor ///////////////////////////////////////////////////

template<typename T>
RGBColor<T>::RGBColor()
  {
  m_rgb[0]=0;
  m_rgb[1]=0;
  m_rgb[2]=0;
  }

template<typename T>
RGBColor<T>::RGBColor(T i_r, T i_g, T i_b)
  {
  ASSERT(IsNaN(i_r)==false && IsNaN(i_g)==false && IsNaN(i_b)==false);

  m_rgb[0]=i_r;
  m_rgb[1]=i_g;
  m_rgb[2]=i_b;
  }

template<typename T>
T RGBColor<T>::operator[](unsigned char i_index) const
  {
  ASSERT(i_index>=0 && i_index<3);
  return m_rgb[i_index];
  }

template<typename T>
T &RGBColor<T>::operator[](unsigned char i_index)
  {
  ASSERT(i_index>=0 && i_index<3);
  return m_rgb[i_index];
  }

template<typename T>
void RGBColor<T>::Clamp(T i_low, T i_high)
  {
  ASSERT(i_low<=i_high);
  m_rgb[0]=MathRoutines::Clamp(m_rgb[0], i_low, i_high);
  m_rgb[1]=MathRoutines::Clamp(m_rgb[1], i_low, i_high);
  m_rgb[2]=MathRoutines::Clamp(m_rgb[2], i_low, i_high);
  }

template<typename T>
bool InRange(const RGBColor<T> &i_color, T i_low, T i_high)
  {
  return
    i_color[0]>=i_low && i_color[0]<=i_high &&
    i_color[1]>=i_low && i_color[1]<=i_high &&
    i_color[2]>=i_low && i_color[2]<=i_high;
  }

/**
* Serializes RGBColor to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<typename T, class Archive>
void serialize(Archive &i_ar, RGBColor<T> &i_color, const unsigned int i_version)
  {
  i_ar & i_color[0];
  i_ar & i_color[1];
  i_ar & i_color[2];
  }

// Don't store class info for RGBColor.
BOOST_CLASS_IMPLEMENTATION(RGBColor_f, boost::serialization::object_serializable)
BOOST_CLASS_IMPLEMENTATION(RGBColor_d, boost::serialization::object_serializable)

////////////////////////////////////////////// XYZColor ///////////////////////////////////////////////////

template<typename T>
XYZColor<T>::XYZColor()
  {
  m_xyz[0]=0;
  m_xyz[1]=0;
  m_xyz[2]=0;
  }

template<typename T>
XYZColor<T>::XYZColor(T i_x, T i_y, T i_z)
  {
  ASSERT(IsNaN(i_x)==false && IsNaN(i_y)==false && IsNaN(i_z)==false);

  m_xyz[0]=i_x;
  m_xyz[1]=i_y;
  m_xyz[2]=i_z;
  }

template<typename T>
T XYZColor<T>::operator[](unsigned char i_index) const
  {
  ASSERT(i_index>=0 && i_index<3);
  return m_xyz[i_index];
  }

template<typename T>
T &XYZColor<T>::operator[](unsigned char i_index)
  {
  ASSERT(i_index>=0 && i_index<3);
  return m_xyz[i_index];
  }

template<typename T>
void XYZColor<T>::Clamp(T i_low, T i_high)
  {
  ASSERT(i_low<=i_high);
  m_xyz[0]=MathRoutines::Clamp(m_xyz[0], i_low, i_high);
  m_xyz[1]=MathRoutines::Clamp(m_xyz[1], i_low, i_high);
  m_xyz[2]=MathRoutines::Clamp(m_xyz[2], i_low, i_high);
  }

template<typename T>
bool InRange(const XYZColor<T> &i_color, T i_low, T i_high)
  {
  return
    i_color[0]>=i_low && i_color[0]<=i_high &&
    i_color[1]>=i_low && i_color[1]<=i_high &&
    i_color[2]>=i_low && i_color[2]<=i_high;
  }

/**
* Serializes XYZColor to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<typename T, class Archive>
void serialize(Archive &i_ar, XYZColor<T> &i_color, const unsigned int i_version)
  {
  i_ar & i_color[0];
  i_ar & i_color[1];
  i_ar & i_color[2];
  }

// Don't store class info for XYZColor.
BOOST_CLASS_IMPLEMENTATION(XYZColor_f, boost::serialization::object_serializable)
BOOST_CLASS_IMPLEMENTATION(XYZColor_d, boost::serialization::object_serializable)


///////////////////////////////////////////// ColorSystem //////////////////////////////////////////////////

inline ColorSystem::ColorSystem():
m_red(1.0,0.0), m_green(0.0,1.0), m_blue(0.0,0.0), m_white(1.0/3.0,1.0/3.0), m_gamma(1.0), m_inv_gamma(1.0), m_XYZ_To_RGB(true), m_RGB_To_XYZ(true)
  {
  }

inline ColorSystem::ColorSystem(Point2D_d i_red, Point2D_d i_green, Point2D_d i_blue, Point2D_d i_white, double i_gamma):
m_red(i_red), m_green(i_green), m_blue(i_blue), m_white(i_white), m_gamma(i_gamma)
  {
  ASSERT(i_gamma>0.0);
  m_inv_gamma = 1.0/i_gamma;

  /// The math and implementation was taken from the luxrender.
  double red[3] = {i_red[0] / i_red[1], 1.0, (1.0 - i_red[0] - i_red[1]) / i_red[1]};
  double green[3] = {i_green[0] / i_green[1], 1.0, (1.0 - i_green[0] - i_green[1]) / i_green[1]};
  double blue[3] = {i_blue[0] / i_blue[1], 1.0, (1.0 - i_blue[0] - i_blue[1]) / i_blue[1]};
  double white[3] = {i_white[0] / i_white[1], 1.0, (1.0 - i_white[0] - i_white[1]) / i_white[1]};

  Matrix3x3_d rgb;
  rgb.m_values[0][0] = red[0];   rgb.m_values[1][0] = red[1];   rgb.m_values[2][0] = red[2];
  rgb.m_values[0][1] = green[0]; rgb.m_values[1][1] = green[1]; rgb.m_values[2][1] = green[2];
  rgb.m_values[0][2] = blue[0];  rgb.m_values[1][2] = blue[1];  rgb.m_values[2][2] = blue[2];
  bool invertible = rgb.Inverted(rgb);
  ASSERT(invertible);

  double y[3];
  rgb.Multiply(white, y);
  double x[3] = {y[0] * red[0], y[1] * green[0], y[2] * blue[0]};
  double z[3] = {y[0] * red[2], y[1] * green[2], y[2] * blue[2]};

  rgb.m_values[0][0] = x[0] + white[0]; rgb.m_values[1][0] = x[1] + white[0]; rgb.m_values[2][0] = x[2] + white[0];
  rgb.m_values[0][1] = y[0] + white[1]; rgb.m_values[1][1] = y[1] + white[1]; rgb.m_values[2][1] = y[2] + white[1];
  rgb.m_values[0][2] = z[0] + white[2]; rgb.m_values[1][2] = z[1] + white[2]; rgb.m_values[2][2] = z[2] + white[2];

  Matrix3x3_d M;
  M.m_values[0][0] = (_Dot(x, x) + white[0] * white[0]);
  M.m_values[1][0] = (_Dot(x, y) + white[1] * white[0]);
  M.m_values[2][0] = (_Dot(x, z) + white[2] * white[0]);
  M.m_values[0][1] = (_Dot(y, x) + white[0] * white[1]);
  M.m_values[1][1] = (_Dot(y, y) + white[1] * white[1]);
  M.m_values[2][1] = (_Dot(y, z) + white[2] * white[1]);
  M.m_values[0][2] = (_Dot(z, x) + white[0] * white[2]);
  M.m_values[1][2] = (_Dot(z, y) + white[1] * white[2]);
  M.m_values[2][2] = (_Dot(z, z) + white[2] * white[2]);
  invertible = M.Inverted(M);
  ASSERT(invertible);

  m_XYZ_To_RGB = rgb;
  m_XYZ_To_RGB.PostMultiply(M);
  invertible = m_XYZ_To_RGB.Inverted(m_RGB_To_XYZ);
  ASSERT(invertible);
  }

inline Point2D_d ColorSystem::GetRedPrimaryPoint() const
  {
  return m_red;
  }

inline Point2D_d ColorSystem::GetGreenPrimaryPoint() const
  {
  return m_green;
  }

inline Point2D_d ColorSystem::GetBluePrimaryPoint() const
  {
  return m_blue;
  }

inline Point2D_d ColorSystem::GetWhitePoint() const
  {
  return m_white;
  }

inline double ColorSystem::GetGamma() const
  {
  return m_gamma;
  }

inline double ColorSystem::_Dot(const double i_a[3], const double i_b[3]) const
  {
  return i_a[0] * i_b[0] + i_a[1] * i_b[1] + i_a[2] * i_b[2];
  }

template<typename T>
RGBColor<T> ColorSystem::XYZ_To_RGB(const XYZColor<T> &i_xyz_color, bool i_constrain) const
  {
  double rgb[3];
  double xyz[3]={i_xyz_color[0], i_xyz_color[1], i_xyz_color[2]};
  m_XYZ_To_RGB.Multiply(xyz, rgb);

  // Constrain the RGB color if needed.
  // This is done by desaturating the color until it lies on the edge of the RGB triangle.
  // Desaturation is done by linearly interpolating between the color and the white point of the same luminance.
  if (i_constrain==false)
    return RGBColor<T>((T)rgb[0], (T)rgb[1], (T)rgb[2]);

  if (rgb[0]>=0.0 && rgb[1]>=0.0 && rgb[2]>=0.0)
    return RGBColor<T>((T)rgb[0], (T)rgb[1], (T)rgb[2]);

  double luminance = i_xyz_color[1];
  if (luminance < 0.0)
    return RGBColor<T>((T)0, (T)0, (T)0);

  double t;
  if (rgb[0] < rgb[1] && rgb[0] < rgb[2])
    t = luminance / (luminance - rgb[0]);
  else if (rgb[1] < rgb[2])
    t = luminance / (luminance - rgb[1]);
  else
    t = luminance / (luminance - rgb[2]);

  // Linearly interpolate between the white point and the point which is out-of-gamut.
  ASSERT(t>=0.0 && t<=1.0);
  rgb[0] = (T) (luminance*(1.0-t) + rgb[0]*t);
  rgb[1] = (T) (luminance*(1.0-t) + rgb[1]*t);
  rgb[2] = (T) (luminance*(1.0-t) + rgb[2]*t);

  ASSERT (rgb[0]>-DBL_EPS && rgb[1]>-DBL_EPS && rgb[2]>-DBL_EPS);
  return RGBColor<T>((T)rgb[0], (T)rgb[1], (T)rgb[2]);
  }

template<typename T>
XYZColor<T> ColorSystem::RGB_To_XYZ(const RGBColor<T> &i_rgb_color) const
  {
  double rgb[3]={i_rgb_color[0], i_rgb_color[1], i_rgb_color[2]};
  double xyz[3];
  m_RGB_To_XYZ.Multiply(rgb, xyz);
  return XYZColor<T>((T)xyz[0], (T)xyz[1], (T)xyz[2]);
  }

template<typename T>
RGBColor<T> ColorSystem::GammaEncode(const RGBColor<T> &i_rgb_color) const
  {
  ASSERT(InRange(i_rgb_color, (T)0.0, (T)1.0));

  RGBColor<T> ret=i_rgb_color;
  ret.m_rgb[0] = (T)pow((double)ret.m_rgb[0], m_inv_gamma);
  ret.m_rgb[1] = (T)pow((double)ret.m_rgb[1], m_inv_gamma);
  ret.m_rgb[2] = (T)pow((double)ret.m_rgb[2], m_inv_gamma);
  return ret;
  }

template<typename T>
RGBColor<T> ColorSystem::GammaDecode(const RGBColor<T> &i_rgb_color) const
  {
  ASSERT(InRange(i_rgb_color, (T)0.0, (T)1.0));

  RGBColor<T> ret=i_rgb_color;
  ret.m_rgb[0] = (T)pow((double)ret.m_rgb[0], m_gamma);
  ret.m_rgb[1] = (T)pow((double)ret.m_rgb[1], m_gamma);
  ret.m_rgb[2] = (T)pow((double)ret.m_rgb[2], m_gamma);
  return ret;
  }

/**
* Serializes ColorSystem to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void ColorSystem::serialize(Archive &i_ar, const unsigned int i_version)
  {
  i_ar & m_red;
  i_ar & m_green;
  i_ar & m_blue;
  i_ar & m_white;

  i_ar & m_XYZ_To_RGB;
  i_ar & m_RGB_To_XYZ;

  i_ar & m_gamma;
  i_ar & m_inv_gamma;
  }

#endif // COLOR_H