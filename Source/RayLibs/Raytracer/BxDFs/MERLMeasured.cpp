/*
* Copyright (C) 2014 by Volodymyr Kachurovskyi <Volodymyr.Kachurovskyi@gmail.com>
*
* This file is part of Skwarka.
*
* Skwarka is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
*
* Skwarka is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with Skwarka.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "MERLMeasured.h"
#include <Math/Constants.h>
#include <Math/MathRoutines.h>
#include <Raytracer/Core/Color.h>
#include <Raytracer/Core/SpectrumRoutines.h>
#include <algorithm>

/////////////////////////////////// MERLMeasuredData::Segmentation2D //////////////////////////////////////

MERLMeasuredData::Segmentation2D::Segmentation2D(const std::vector<std::vector<float>> &i_values)
{
  size_t size_x = SEGMENTATION_EXITANT_PHI_RES, size_y = SEGMENTATION_EXITANT_THETA_RES;
  ASSERT(i_values.size()>=size_y && i_values[0].size()>=size_x);
  size_t n=i_values.size(), m=i_values[0].size();

  std::vector<float> sum_X(m), sum_Y(n);
  for(size_t i=0;i<n;++i)
    for(size_t j=0;j<m;++j)
      {
      sum_X[j] += i_values[i][j];
      sum_Y[i] += i_values[i][j];
      }

  std::vector<size_t> reduction_X = _Reduce(sum_X, size_x);
  std::vector<size_t> reduction_Y = _Reduce(sum_Y, size_y);

  // Normalize X grid line coordinates.
  m_grid_X.assign(size_x, 0.f);
  for(size_t i=0;i<size_x;++i)
    m_grid_X[i] = reduction_X[i] / (float)m;

  // Normalize Y grid line coordinates.
  m_grid_Y.assign(size_y, 0.f);
  for(size_t i=0;i<size_y;++i)
    m_grid_Y[i] = reduction_Y[i] / (float)n;

  // "Reduce" the original 2D array to a new one of the smaller size by summing the original value in each cell of the "reduced" grid.
  // The values in each row are also progressively summed up to build the CDF function.
  for(size_t i=0;i<size_y;++i)
    for(size_t j=0;j<size_x;++j)
      {
      if (j>0)
        m_CDF_cols[i][j]=m_CDF_cols[i][j-1];
      else
        m_CDF_cols[i][j]=0.f;

      size_t end_y = (i+1<size_y) ? reduction_Y[i+1] : n;
      size_t end_x = (j+1<size_x) ? reduction_X[j+1] : m;
      for(size_t i1=reduction_Y[i];i1<end_y;++i1)
        for(size_t j1=reduction_X[j];j1<end_x;++j1)
          m_CDF_cols[i][j] += i_values[i1][j1];
      }

  // Build CDF for rows.
  m_CDF_rows.assign(size_y, 0.f);
  for(size_t i=0;i<size_y;++i)
    {
    if (i>0) m_CDF_rows[i]=m_CDF_rows[i-1];
    m_CDF_rows[i] += m_CDF_cols[i][size_x-1];
    }

  // Normalize CDF values.
  for(size_t i=0;i<size_y;++i)
    {
    m_CDF_rows[i] /= m_CDF_rows.back();

    double inv = 1.0/m_CDF_cols[i][size_x-1];
    for(size_t j=0;j<size_x;++j)
      m_CDF_cols[i][j] = (float)(m_CDF_cols[i][j]*inv);
    }
  }

/*
* Reduces the input array to the specified size by merging adjacent elements.
* On each step the two adjacent merged elements are the ones whose sum is the minimal.
*/
std::vector<size_t> MERLMeasuredData::Segmentation2D::_Reduce(std::vector<float> i_values, size_t i_size) const
  {
  int iterations = (int)i_values.size() - (int)i_size;
  ASSERT(iterations>=0 && i_size>0);

  std::vector<size_t> ret(i_values.size());
  for(size_t i=0;i<i_values.size();++i) ret[i]=i;

  for(int it=0;it<iterations;++it)
    {
    float best = FLT_INF;
    size_t ind = 0;
    for(size_t j=0;j+1<ret.size();++j)
      {
      float tmp = i_values[j]+i_values[j+1];
      if (tmp<best) {best=tmp;ind=j;}
      }
    ASSERT(best<FLT_INF);
    i_values[ind]+=i_values[ind+1];
    i_values.erase(i_values.begin()+ind+1);
    ret.erase(ret.begin()+ind+1);
    }

  ASSERT(ret.size() == i_size);
  return ret;
  }

void MERLMeasuredData::Segmentation2D::Sample(const Point2D_d &i_sample, Point2D_d &o_residual_sample, Point2D_d &o_box_min, Point2D_d &o_box_max, double &o_pdf) const
  {
  double row_pdf, col_pdf;
  size_t row_index = MathRoutines::BinarySearchCDF(m_CDF_rows.begin(), m_CDF_rows.end(), i_sample[1], &row_pdf) - m_CDF_rows.begin();
  
  const float *p_CDF_cols = m_CDF_cols[row_index];
  size_t col_index = MathRoutines::BinarySearchCDF(p_CDF_cols, p_CDF_cols+SEGMENTATION_EXITANT_PHI_RES, i_sample[0], &col_pdf) - p_CDF_cols;

  o_residual_sample[0] = (i_sample[0] - (col_index>0 ? p_CDF_cols[col_index-1] : 0.0)) / col_pdf;
  o_residual_sample[1] = (i_sample[1] - (row_index>0 ? m_CDF_rows[row_index-1] : 0.0)) / row_pdf;

  o_box_min[0] = m_grid_X[col_index];
  o_box_min[1] = m_grid_Y[row_index];
  o_box_max[0] = col_index+1<SEGMENTATION_EXITANT_PHI_RES ? m_grid_X[col_index+1] : 1.0;
  o_box_max[1] = row_index+1<SEGMENTATION_EXITANT_THETA_RES ? m_grid_Y[row_index+1] : 1.0;

  o_pdf = row_pdf*col_pdf;
  }

void MERLMeasuredData::Segmentation2D::PDF(const Point2D_d &i_point, Point2D_d &o_box_min, Point2D_d &o_box_max, double &o_pdf) const
  {
  ASSERT(i_point[0]>=0.0 && i_point[0]<1.0);
  ASSERT(i_point[1]>=0.0 && i_point[1]<1.0);

  size_t col_index = std::upper_bound(m_grid_X.begin(), m_grid_X.end(), i_point[0]) - m_grid_X.begin();
  size_t row_index = std::upper_bound(m_grid_Y.begin(), m_grid_Y.end(), i_point[1]) - m_grid_Y.begin();
  ASSERT(row_index>0 && col_index>0);
  --col_index; --row_index;

  const float *p_CDF_cols = m_CDF_cols[row_index];
  o_box_min[0] = m_grid_X[col_index];
  o_box_min[1] = m_grid_Y[row_index];
  o_box_max[0] = (size_t)(col_index+1)<SEGMENTATION_EXITANT_PHI_RES ? m_grid_X[col_index+1] : 1.0;
  o_box_max[1] = (size_t)(row_index+1)<SEGMENTATION_EXITANT_THETA_RES ? m_grid_Y[row_index+1] : 1.0;

  double row_pdf = row_index>0 ? m_CDF_rows[row_index] - m_CDF_rows[row_index-1] : m_CDF_rows[row_index];
  double col_pdf = col_index>0 ? p_CDF_cols[col_index] - p_CDF_cols[col_index-1] : p_CDF_cols[col_index];
  o_pdf = row_pdf*col_pdf;
  }

////////////////////////////////////////// MERLMeasuredData ///////////////////////////////////////////////

MERLMeasuredData::MERLMeasuredData(std::istream &i_stream)
  {
  int dims[3];
  i_stream.read(reinterpret_cast<char*>(&dims[0]), sizeof(int));
  i_stream.read(reinterpret_cast<char*>(&dims[1]), sizeof(int));
  i_stream.read(reinterpret_cast<char*>(&dims[2]), sizeof(int));
  if (i_stream.eof() || i_stream.fail())
    {
    ASSERT(0 && "Error during reading input stream in MERLMeasuredData::MERLMeasuredData().");
    return;
    }

  size_t n = dims[0] * dims[1] * dims[2];
  if (n != BRDF_SAMPLING_RES_THETA_H*BRDF_SAMPLING_RES_THETA_D*BRDF_SAMPLING_RES_PHI_D) 
    {
    ASSERT(0 && "Dimensions don't match in MERLMeasuredData::MERLMeasuredData().");
    return;
    }

  double *p_regular_halfangle_data = new double[3*n];

  const size_t chunk_size = 2*BRDF_SAMPLING_RES_PHI_D;
  char *p_tmp = new char[chunk_size*sizeof(double)];
  size_t num_chunks = n / chunk_size;
  ASSERT((n % chunk_size) == 0);
  double scales[3] = {1.0/1500, 1.15/1500, 1.66/1500};

  for (unsigned char c=0;c<3;++c)
    {
    int offset = 0;
    for (size_t i=0;i<num_chunks;++i)
      {
      i_stream.read(p_tmp, chunk_size*sizeof(double));
      if (i_stream.eof() || i_stream.fail() || i_stream.gcount() != chunk_size*sizeof(double))
        {
        ASSERT(0 && "Premature end-of-file in MERLMeasuredData::MERLMeasuredData().");
        delete[] p_tmp;
        delete[] p_regular_halfangle_data;
        return;
        }

      for (size_t j=0;j<chunk_size;++j)
        {
        double val = *reinterpret_cast<double*>(p_tmp+j*sizeof(double));
        p_regular_halfangle_data[3*(offset++) + c] = std::max(0.0, val*scales[c]);
        }
      }
    }

  m_brdf_data.resize(n);
  for (size_t i=0;i<n;++i)
    {
    RGBColor_d rgb(p_regular_halfangle_data[3*i+0], p_regular_halfangle_data[3*i+1], p_regular_halfangle_data[3*i+2]);
    XYZColor_d xyz = global_sRGB_E_ColorSystem.RGB_To_XYZ(rgb);
    m_brdf_data[i] =  Convert<HalfFloat>( Convert<float>( SpectrumRoutines::XYZToSpectrumCoef(xyz) ) );
    }

  delete[] p_tmp;
  delete[] p_regular_halfangle_data;

  _InitializeSegmentations();
  }

void MERLMeasuredData::_InitializeSegmentations()
  {
  // Originally we sample the BRDF values at a higher frequency and then reduce it back to the needed resolution.
  // The point is that we are able to choose the positions of the grid lines more wisely when having the data with a higher resolution.
  const size_t exitant_theta_res = SEGMENTATION_MULTIPLIER*SEGMENTATION_EXITANT_THETA_RES;
  const size_t exitant_phi_res = SEGMENTATION_MULTIPLIER*SEGMENTATION_EXITANT_PHI_RES;
  double phi_coef = M_PI / (exitant_phi_res), theta_coef = M_PI_2 / (exitant_theta_res);

  std::vector<std::vector<float>> values(exitant_theta_res, std::vector<float>(exitant_phi_res));
  for(size_t i=0;i<SEGMENTATION_INCIDENT_THETA_RES;++i)
    {
    double incident_theta = (i+0.5)*M_PI_2/SEGMENTATION_INCIDENT_THETA_RES;
    Vector3D_d incident = MathRoutines::SphericalDirection<double>(0.0, incident_theta);

    for(size_t i1=0;i1<exitant_theta_res;++i1)
      {
      double exitant_theta = (i1+0.5)*theta_coef;
      double cos_exitant_theta = cos(exitant_theta);
      double cos_theta1 = cos(i1*theta_coef), cos_theta2 = cos((i1+1)*theta_coef);
      double d_cos_theta = cos_theta1 - cos_theta2;

      for(size_t j1=0;j1<exitant_phi_res;++j1)
        {
        double luminance = SpectrumRoutines::Luminance(GetBRDF(incident, MathRoutines::SphericalDirection<double>((j1+0.5)*phi_coef, exitant_theta)));
        values[i1][j1] = (float)(phi_coef*d_cos_theta*cos_exitant_theta*luminance);
        }
      }

    m_segmentations.push_back( Segmentation2D(values) );
    }
  }

SpectrumCoef_d MERLMeasuredData::GetBRDF(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const
  {
  if (m_brdf_data.empty())
    return SpectrumCoef_d();

  Vector3D_d half_angle = i_incident+i_exitant;
  if (half_angle[0] == 0.0 && half_angle[1] == 0.0 && half_angle[2] == 0.0)
    return SpectrumCoef_d();

  half_angle.Normalize();
  double sin_theta, cos_theta, sin_phi, cos_phi;
  MathRoutines::SphericalAngles(half_angle, sin_theta, cos_theta, sin_phi, cos_phi);
  Vector3D_d whx(cos_phi * cos_theta, sin_phi * cos_theta, -sin_theta);
  Vector3D_d why(-sin_phi, cos_phi, 0);
  Vector3D_d diff_vector(i_incident*whx, i_incident*why, i_incident*half_angle);

  double half_vector_theta = MathRoutines::SphericalTheta(half_angle);
  double diff_vector_theta = MathRoutines::SphericalTheta(diff_vector), diff_vector_phi = MathRoutines::SphericalPhi(diff_vector);
  ASSERT(half_vector_theta >= 0.0 && diff_vector_phi >= 0.0 && diff_vector_theta >= 0.0);

  // Because of reciprocity, the BRDF is unchanged under diff_vector_phi -> diff_vector_phi - M_PI
  // Instead of subtracting M_PI from diff_vector_phi (if needed) we just take the computed index modulo BRDF_SAMPLING_RES_PHI_D
  size_t diff_vector_phi_index = size_t(diff_vector_phi * INV_PI * BRDF_SAMPLING_RES_PHI_D) % BRDF_SAMPLING_RES_PHI_D;
  size_t diff_vector_theta_index = std::min(size_t(diff_vector_theta * 2.0 * INV_PI * BRDF_SAMPLING_RES_THETA_D), BRDF_SAMPLING_RES_THETA_D - 1);

  double half_vector_theta_deg_scaled = sqrt(half_vector_theta * 2.0 * INV_PI) * BRDF_SAMPLING_RES_THETA_H;
  size_t half_vector_theta_index = std::min(size_t(half_vector_theta_deg_scaled), BRDF_SAMPLING_RES_THETA_H - 1);

  size_t index = diff_vector_phi_index + (diff_vector_theta_index + half_vector_theta_index * BRDF_SAMPLING_RES_THETA_D) * BRDF_SAMPLING_RES_PHI_D;
  ASSERT(index>=0 && index<BRDF_SAMPLING_RES_THETA_H*BRDF_SAMPLING_RES_THETA_D*BRDF_SAMPLING_RES_PHI_D);
   
  if (index < (BRDF_SAMPLING_RES_THETA_H-1)*BRDF_SAMPLING_RES_THETA_D*BRDF_SAMPLING_RES_PHI_D)
    {
    // Interpolate by the theta angle of half vector.
    double t = half_vector_theta_deg_scaled-(size_t)half_vector_theta_deg_scaled;
    return Convert<double>(m_brdf_data[index])*(1.0-t) + Convert<double>(m_brdf_data[index+BRDF_SAMPLING_RES_THETA_D*BRDF_SAMPLING_RES_PHI_D])*t;
    }
  else
    return Convert<double>( m_brdf_data[index] );
  }

SpectrumCoef_d MERLMeasuredData::Sample(const Vector3D_d &i_incident, Vector3D_d &o_exitant, const Point2D_d &i_sample, double &o_pdf) const
  {
  if (m_brdf_data.empty())
    {
    o_pdf=0.0;
    return SpectrumCoef_d();
    }

  size_t incident_theta_index = std::min((size_t)(MathRoutines::SphericalTheta(i_incident)*2.0*INV_PI * SEGMENTATION_INCIDENT_THETA_RES), SEGMENTATION_INCIDENT_THETA_RES-1);

  Point2D_d sample(i_sample);
  bool opposite = false;
  if (sample[0]>0.5)
    {
    opposite = true;
    sample[0] = (sample[0]-0.5)*2.0;
    }
  else
    sample[0] *= 2.0;

  double pdf;
  Point2D_d residual_sample, box_min, box_max;
  m_segmentations[incident_theta_index].Sample(sample, residual_sample, box_min, box_max, pdf);
  
  double d_phi = (box_max[0]-box_min[0]) * M_PI;
  double cos_theta1 = cos(box_min[1]*M_PI_2), cos_theta2 = cos(box_max[1]*M_PI_2);
  double d_cos_theta = cos_theta1 - cos_theta2;
  double pixel_solid_angle = d_phi*d_cos_theta;

  double exitant_phi = (box_min[0]+(box_max[0]-box_min[0])*residual_sample[0]) * M_PI;
  if (opposite) exitant_phi = 2.0*M_PI - exitant_phi;
  exitant_phi += MathRoutines::SphericalPhi(i_incident);

  double exitant_theta_cos = cos_theta1-residual_sample[1]*d_cos_theta;
  double exitant_theta_sin = sqrt(1.0-exitant_theta_cos*exitant_theta_cos);

  o_exitant = Vector3D_d(exitant_theta_sin*cos(exitant_phi), exitant_theta_sin*sin(exitant_phi), exitant_theta_cos);

  o_pdf = 0.5*pdf / pixel_solid_angle;
  return GetBRDF(i_incident, o_exitant);
  }

double MERLMeasuredData::PDF(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const
  {
  if (m_brdf_data.empty())
    return 0.0;

  size_t incident_theta_index = std::min((size_t)(MathRoutines::SphericalTheta(i_incident)*2.0*INV_PI * SEGMENTATION_INCIDENT_THETA_RES), SEGMENTATION_INCIDENT_THETA_RES-1);
  
  double incident_phi = MathRoutines::SphericalPhi(i_incident);
  double exitant_phi = MathRoutines::SphericalPhi(i_exitant);
  double exitant_theta = MathRoutines::SphericalTheta(i_exitant);
  double delta_phi = fabs(incident_phi-exitant_phi);
  if (delta_phi>M_PI) delta_phi = 2.0*M_PI - delta_phi;

  Point2D_d point(delta_phi*INV_PI, exitant_theta*2.0*INV_PI);

  double pdf;
  Point2D_d box_min, box_max;
  m_segmentations[incident_theta_index].PDF(point, box_min, box_max, pdf);

  double d_phi = (box_max[0]-box_min[0]) * M_PI;
  double cos_theta1 = cos(box_min[1]*M_PI_2), cos_theta2 = cos(box_max[1]*M_PI_2);
  double d_cos_theta = cos_theta1 - cos_theta2;
  double pixel_solid_angle = d_phi*d_cos_theta;

  return 0.5*pdf / pixel_solid_angle;
  }

//////////////////////////////////////////// MERLMeasured /////////////////////////////////////////////////

MERLMeasured::MERLMeasured(const MERLMeasuredData *ip_merl_measured_data):
BxDF(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY)), mp_merl_measured_data(ip_merl_measured_data)
  {
  ASSERT(ip_merl_measured_data);
  }

SpectrumCoef_d MERLMeasured::Evaluate(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const
  {
  ASSERT(i_incident.IsNormalized());
  ASSERT(i_exitant.IsNormalized());

  if (i_incident[2]*i_exitant[2] < 0.0)
    return SpectrumCoef_d();
  else
    if (i_incident[2] >= 0.0)
      return mp_merl_measured_data->GetBRDF(i_incident, i_exitant);
    else
      {
      Vector3D_d incident(i_incident[0], i_incident[1], -i_incident[2]);
      Vector3D_d exitant(i_exitant[0], i_exitant[1], -i_exitant[2]);
      return mp_merl_measured_data->GetBRDF(incident, exitant);
      }
  }

SpectrumCoef_d MERLMeasured::Sample(const Vector3D_d &i_incident, Vector3D_d &o_exitant, const Point2D_d &i_sample, double &o_pdf) const
  {
  ASSERT(i_sample[0]>=0.0 && i_sample[0]<=1.0);
  ASSERT(i_sample[1]>=0.0 && i_sample[1]<=1.0);
  ASSERT(i_incident.IsNormalized());

  if (i_incident[2] >= 0.0)
    return mp_merl_measured_data->Sample(i_incident, o_exitant, i_sample, o_pdf);
  else
    {
    Vector3D_d incident(i_incident[0], i_incident[1], -i_incident[2]);
    SpectrumCoef_d ret = mp_merl_measured_data->Sample(incident, o_exitant, i_sample, o_pdf);
    o_exitant[2] *= -1.0;
    return ret;
    }
  }

double MERLMeasured::PDF(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const
  {
  ASSERT(i_incident.IsNormalized());
  ASSERT(i_exitant.IsNormalized());

  if (i_incident[2]*i_exitant[2] < 0.0)
    return 0.0;
  else
    if (i_incident[2] >= 0.0)
      return mp_merl_measured_data->PDF(i_incident, i_exitant);
    else
      {
      Vector3D_d incident(i_incident[0], i_incident[1], -i_incident[2]);
      Vector3D_d exitant(i_exitant[0], i_exitant[1], -i_exitant[2]);
      return mp_merl_measured_data->PDF(incident, exitant);
      }
  }