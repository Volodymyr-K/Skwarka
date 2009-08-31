#ifndef SAMPLE_H
#define SAMPLE_H

#include <Common\Common.h>
#include <Math\Geometry.h>
#include <vector>

struct SamplesSequence1D 
  {
  typedef std::vector<double>::iterator IteratorType;

  SamplesSequence1D(IteratorType i_begin, IteratorType i_end);

  IteratorType m_begin, m_end;
  };

struct SamplesSequence2D 
  {
  typedef std::vector<Point2D_d>::iterator IteratorType;

  SamplesSequence2D(IteratorType i_begin, IteratorType i_end);

  IteratorType m_begin, m_end;
  };

class Sample
  { 
  public:
    Sample(const std::vector<size_t> &i_sequences_1D_size, const std::vector<size_t> &i_sequences_2D_size);

    void SetImagePoint(const Point2D_d &i_image_point);
    Point2D_d GetImagePoint() const;

    void SetLensUV(const Point2D_d &i_lens_uv);
    Point2D_d GetLensUV() const;

    size_t GetNumberOfSamplesSequences1D() const;
    SamplesSequence1D GetSamplesSequence1D(size_t i_index) const;

    size_t GetNumberOfSamplesSequences2D() const;
    SamplesSequence2D GetSamplesSequence2D(size_t i_index) const;

  private:
    // not implemented, Samples should only be passed by a reference to avoid large data copying
    Sample();
    Sample(const Sample&);
    Sample &operator=(const Sample&);

  private:
    Point2D_d m_image_point;
    Point2D_d m_lens_uv;

    std::vector<double> m_1D_samples;
    std::vector<Point2D_d> m_2D_samples;

    std::vector<SamplesSequence1D> m_sample_sequences_1D;
    std::vector<SamplesSequence2D> m_sample_sequences_2D;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline SamplesSequence1D::SamplesSequence1D(SamplesSequence1D::IteratorType i_begin, SamplesSequence1D::IteratorType i_end):
m_begin(i_begin), m_end(i_end)
  {
  }

inline SamplesSequence2D::SamplesSequence2D(SamplesSequence2D::IteratorType i_begin, SamplesSequence2D::IteratorType i_end):
m_begin(i_begin), m_end(i_end)
  {
  }

inline Sample::Sample(const std::vector<size_t> &i_sequences_1D_size, const std::vector<size_t> &i_sequences_2D_size)
  {
  // 1D
  size_t total_1D_samples=0;
  for(size_t i=0;i<i_sequences_1D_size.size();++i)
    total_1D_samples+=i_sequences_1D_size[i];

  m_1D_samples.assign(total_1D_samples, 0.0);

  total_1D_samples=0;
  for(size_t i=0;i<i_sequences_1D_size.size();++i)
    m_sample_sequences_1D.push_back( SamplesSequence1D(m_1D_samples.begin()+total_1D_samples, m_1D_samples.begin()+total_1D_samples+i_sequences_1D_size[i]) );

  // 2D
  size_t total_2D_samples=0;
  for(size_t i=0;i<i_sequences_2D_size.size();++i)
    total_2D_samples+=i_sequences_2D_size[i];

  m_2D_samples.assign(total_2D_samples, Point2D_d());

  total_2D_samples=0;
  for(size_t i=0;i<i_sequences_2D_size.size();++i)
    m_sample_sequences_2D.push_back( SamplesSequence2D(m_2D_samples.begin()+total_2D_samples, m_2D_samples.begin()+total_2D_samples+i_sequences_2D_size[i]) );
  }

inline void Sample::SetImagePoint(const Point2D_d &i_image_point)
  {
  m_image_point=i_image_point;
  }

inline Point2D_d Sample::GetImagePoint() const
  {
  return m_image_point;
  }

inline void Sample::SetLensUV(const Point2D_d &i_lens_uv)
  {
  m_lens_uv=i_lens_uv;
  }

inline Point2D_d Sample::GetLensUV() const
  {
  return m_lens_uv;
  }

inline size_t Sample::GetNumberOfSamplesSequences1D() const
  {
  return m_sample_sequences_1D.size();
  }

inline SamplesSequence1D Sample::GetSamplesSequence1D(size_t i_index) const
  {
  ASSERT(i_index<m_sample_sequences_1D.size());
  return m_sample_sequences_1D[i_index];
  }

inline size_t Sample::GetNumberOfSamplesSequences2D() const
  {
  return m_sample_sequences_2D.size();
  }

inline SamplesSequence2D Sample::GetSamplesSequence2D(size_t i_index) const
  {
  ASSERT(i_index<m_sample_sequences_2D.size());
  return m_sample_sequences_2D[i_index];
  }

#endif // SAMPLE_H