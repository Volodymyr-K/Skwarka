#ifndef SAMPLER_MOCK_H
#define SAMPLER_MOCK_H

#include <Common/Common.h>
#include <Raytracer/Core/Sample.h>
#include <Raytracer/Core/Sampler.h>
#include <Math/Geometry.h>

/*
Sampler mock implementation.
Always generates constant samples.
*/
class SamplerMock: public Sampler
  {
  public:
    SamplerMock(const Point2D_i &i_image_begin, const Point2D_i &i_image_end, size_t i_samples_per_pixel): Sampler(i_image_begin, i_image_end, i_samples_per_pixel)
      {
      }

  protected:
    size_t _RoundSamplesNumber(size_t i_samples_number) const
      {
      return i_samples_number;
      }

    void _GetSample(const Point2D_i &i_current_pixel, size_t i_pixel_sample_index, intrusive_ptr<Sample> op_sample)
      {
      op_sample->SetImagePoint(Point2D_d(0.5,0.5));
      op_sample->SetLensUV(Point2D_d(0.5,0.5));

      for(size_t i=0;i<op_sample->GetNumberOfSamplesSequences1D();++i)
        {
        SamplesSequence1D sequence = op_sample->GetSamplesSequence1D(i);
        std::fill(sequence.m_begin, sequence.m_end, 0.5);
        }

      for(size_t i=0;i<op_sample->GetNumberOfSamplesSequences2D();++i)
        {
        SamplesSequence2D sequence = op_sample->GetSamplesSequence2D(i);
        std::fill(sequence.m_begin, sequence.m_end, Point2D_d(0.5, 0.5));
        }
      }
  };

#endif // SAMPLER_MOCK_H