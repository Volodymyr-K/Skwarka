#ifndef COLOR_H
#define COLOR_H

#include <Common/Common.h>
#include <Math/Matrix3x3.h>
#include <Math/Point2D.h>

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

  T m_xyz[3];
  };

typedef RGBColor<float> RGBColor_f;
typedef RGBColor<double> RGBColor_d;

typedef XYZColor<float> XYZColor_f;
typedef XYZColor<double> XYZColor_d;

/**
* Represents color system defined by the CIE coordinates of its three primary colors and the coordinates of the white point.
* The class provides methods to convert between RGB and XYZ color spaces.
*/
class ColorSystem
  {
  public:
    /**
    * Creates color system with the specified CIE coordinates of its three primary colors and the coordinates of the white point.
    */
    ColorSystem(Point2D_d i_red, Point2D_d i_green, Point2D_d i_blue, Point2D_d i_white);

    Point2D_d GetRedPrimaryPoint() const;

    Point2D_d GetGreenPrimaryPoint() const;

    Point2D_d GetBluePrimaryPoint() const;

    Point2D_d GetWhitePoint() const;

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

  private:
    double _Dot(const double i_a[3], const double i_b[3]) const;

  private:
    Point2D_d m_red, m_green, m_blue, m_white;
    Matrix3x3_d m_XYZ_To_RGB, m_RGB_To_XYZ;
  };

// Global instance of the ColorSystem that corresponds to the sRGB color space.
extern const ColorSystem global_sRGB_ColorSystem;

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

inline ColorSystem::ColorSystem(Point2D_d i_red, Point2D_d i_green, Point2D_d i_blue, Point2D_d i_white):
m_red(i_red), m_green(i_green), m_blue(i_blue), m_white(i_white)
  {
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

/**
* Saves the data which is needed to construct ColorSystem to the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void save_construct_data(Archive &i_ar, const ColorSystem *ip_color_system, const unsigned int i_version)
  {
  Point2D_d red = ip_color_system->GetRedPrimaryPoint();
  Point2D_d green = ip_color_system->GetGreenPrimaryPoint();
  Point2D_d blue = ip_color_system->GetBluePrimaryPoint();
  Point2D_d white = ip_color_system->GetWhitePoint();

  i_ar << red;
  i_ar << green;
  i_ar << blue;
  i_ar << white;
  }

/**
* Constructs ColorSystem with the data from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void load_construct_data(Archive &i_ar, ColorSystem *ip_color_system, const unsigned int i_version)
  {
  Point2D_d red, green, blue, white;

  i_ar >> red;
  i_ar >> green;
  i_ar >> blue;
  i_ar >> white;

  ::new(ip_color_system)ColorSystem(red, green, blue, white);
  }

/**
* Serializes ColorSystem to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void serialize(Archive &i_ar, ColorSystem &i_color_system, const unsigned int i_version)
  {
  }

#endif // COLOR_H