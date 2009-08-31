#ifndef STRATIFIED_SAMPLER_H
#define STRATIFIED_SAMPLER_H

#include <Common\Common.h>
#include <Raytracer\Core\Sample.h>
#include <Raytracer\Core\Sampler.h>
#include <Math\Geometry.h>
#include <Math\MultiThreadedRandom.h>
#include <vector>

class StratifiedSampler: public Sampler
  {
  public:
    StratifiedSampler(const Point2D_i &i_image_begin, const Point2D_i &i_image_end, size_t i_x_samples_per_pixel, size_t i_y_samples_per_pixel, bool i_jitter_samples=true);

  protected:
    size_t _RoundSamplesNumber(size_t i_samples_number) const;
    void _GetSample(const Point2D_i &i_current_pixel, size_t i_pixel_sample_index, shared_ptr<Sample> op_sample);
    void _PrecomputeSamplesForPixel(const Point2D_i &i_current_pixel);

  private:
    size_t m_x_samples_per_pixel, m_y_samples_per_pixel;
    bool m_jitter_samples;

    std::vector<Point2D_d> m_image_points;
    std::vector<Point2D_d> m_lens_UVs;

  };

#endif // RANDOM_SAMPLER_H