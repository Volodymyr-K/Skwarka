#include "MitchellFilter.h"

// Register the derived class in the boost serialization framework.
BOOST_CLASS_EXPORT_IMPLEMENT(MitchellFilter);

MitchellFilter::MitchellFilter(double i_x_width, double i_y_width, double i_b, double i_c):
FilmFilter(i_x_width, i_y_width), m_b(i_b), m_c(i_c)
  {
  m_inv_x_width = i_x_width>0 ? 1.0/i_x_width : 0.0;
  m_inv_y_width = i_y_width>0 ? 1.0/i_y_width : 0.0;
  }

MitchellFilter::MitchellFilter(double i_x_width, double i_y_width, double i_b):
FilmFilter(i_x_width, i_y_width), m_b(i_b), m_c(0.5-0.5*i_b)
  {
  m_inv_x_width = i_x_width>0 ? 1.0/i_x_width : 0.0;
  m_inv_y_width = i_y_width>0 ? 1.0/i_y_width : 0.0;
  }

double MitchellFilter::Evaluate(double i_x, double i_y) const
  {
  return _Mitchell1D(i_x * m_inv_x_width) * _Mitchell1D(i_y * m_inv_y_width);
  }

void MitchellFilter::GetParameters(double &o_b, double &o_c) const
  {
  o_b=m_b;
  o_c=m_c;
  }