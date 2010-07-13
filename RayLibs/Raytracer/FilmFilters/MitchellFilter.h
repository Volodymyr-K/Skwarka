#ifndef MITCHELL_FILTER_H
#define MITCHELL_FILTER_H

#include <Raytracer/Core/FilmFilter.h>

/**
* Mitchell filter implementation. Mitchell filter does a relatively good job of trading off between ringing (phantom edges next to actual edges in the image) and blurring.
* Mitchell filter has negative lobes and thus special care needs to be taken to make sure the final filtered result is in the correct range.
*/
class MitchellFilter: public FilmFilter
  {
  public:
    /**
    * Creates MitchellFilter with the specified width and B and C parameters.
    * B and C parameters control the behavior of the filter by choosing between ringing, blurring and anisotropy.
    * Values of 1/3 for both parameters are suitable for most of the cases.
    */
    MitchellFilter(double i_x_width, double i_y_width, double i_b, double i_c);

    /**
    * Creates MitchellFilter with the specified width and B parameter.
    * Parameter C of the Mitchell filter is automatically selected to give best results.
    * Smaller value of B lead to blurring while higher values lead to ringing.
     * Values of 1/3 is suitable for most of the cases.
    */
    MitchellFilter(double i_x_width, double i_y_width, double i_b = 1.0/3.0);

    /**
    * Returns the filter value for the specified X and Y coordinates.
    */
    double Evaluate(double i_x, double i_y) const;

    /**
    * Gets B and C parameters of the filter.
    */
    void GetParameters(double &o_b, double &o_c) const;

  private:
    /**
    * Private helper method that computes filter value for 1D.
    */
    double _Mitchell1D(double i_x) const;

  private:
    double m_b, m_c;
    double m_inv_x_width, m_inv_y_width;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline double MitchellFilter::_Mitchell1D(double i_x) const
  {
  i_x = fabs(2.0 * i_x);
  if (i_x > 1.0)
    return ((-m_b - 6*m_c) * i_x*i_x*i_x + (6*m_b + 30*m_c) * i_x*i_x +
    (-12*m_b - 48*m_c) * i_x + (8*m_b + 24*m_c)) * (1.0/6.0);
  else
    return ((12 - 9*m_b - 6*m_c) * i_x*i_x*i_x +
    (-18 + 12*m_b + 6*m_c) * i_x*i_x +
    (6 - 2*m_b)) * (1.0/6.0);
  }

/**
* Saves the data which is needed to construct MitchellFilter to the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void save_construct_data(Archive &i_ar, const MitchellFilter *ip_filter, const unsigned int i_version)
  {
  double x_width = ip_filter->GetXWidth();
  double y_width = ip_filter->GetYWidth();
  double b,c;
  ip_filter->GetParameters(b, c);

  i_ar << x_width;
  i_ar << y_width;
  i_ar << b;
  i_ar << c;
  }

/**
* Constructs MitchellFilter with the data from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void load_construct_data(Archive &i_ar, MitchellFilter *ip_filter, const unsigned int i_version)
  {
  double x_width, y_width;
  double b,c;

  i_ar >> x_width;
  i_ar >> y_width;
  i_ar >> b;
  i_ar >> c;

  ::new(ip_filter)MitchellFilter(x_width, y_width, b, c);
  }

/**
* Serializes MitchellFilter to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void serialize(Archive &i_ar, MitchellFilter &i_filter, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<FilmFilter>(i_filter);
  }

// Register the derived class in the boost serialization framework.
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(MitchellFilter)

#endif // MITCHELL_FILTER_H