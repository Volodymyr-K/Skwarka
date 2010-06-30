#ifndef IMAGE_ENVIRONMENTAL_LIGHT_TEST_H
#define IMAGE_ENVIRONMENTAL_LIGHT_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/Numerics.h>
#include <Raytracer/LightSources/ImageEnvironmentalLight.h>
#include <Math/ThreadSafeRandom.h>
#include <Math/SamplingRoutines.h>
#include <Math/MathRoutines.h>
#include <vector>

class ImageEnvironmentalLightTestSuite : public CxxTest::TestSuite
  {
  public:
    ImageEnvironmentalLightTestSuite()
      {
      m_width = 234;
      m_height = 123;

      m_image.assign(m_height, std::vector<Spectrum_f>(m_width));
      for(size_t y=0;y<m_height;++y)
        for(size_t x=0;x<m_width;++x)
          m_image[y][x]=Spectrum_f((float)x,(float)y,(float)(x+y));

      m_bbox = BBox3D_d(Point3D_d(0,0,0), Point3D_d(10,20,30));
      m_light_to_world = MakeRotation(1.2, Vector3D_d(-1,2,-3).Normalized());      
      mp_light = NULL;
      }

    void setUp()
      {
      // We initialize the light in setUp() instead of the constructor because it uses CompressedDirection class which has a statical initializer which needs to be called first.
      if (mp_light == NULL)
        mp_light.reset(new ImageEnvironmentalLight(m_bbox, m_light_to_world, m_image));
      }

    void tearDown()
      {
      // Nothing to clear.
      }

    void test_ImageEnvironmentalLight_Radiance()
      {
      size_t N=1000;
      for(size_t i=0;i<N;++i)
        {
        Vector3D_d dir = Vector3D_d(RandomDouble(2.0)-1.0, RandomDouble(2.0)-1.0, RandomDouble(2.0)-1.0).Normalized();

        Spectrum_d radiance = mp_light->Radiance(RayDifferential(Ray(Point3D_d(), dir)));
        double phi = MathRoutines::SphericalPhi(m_light_to_world.Inverted()(dir)), theta = MathRoutines::SphericalTheta(m_light_to_world.Inverted()(dir));
        double x = phi*INV_2PI*m_width, y = theta*INV_PI*m_height;

        // Avoid testing cases near the edges where the image wraps.
        if (x>2 && y>2 && x<m_width-2 && y<m_height-2)
          {
          Spectrum_d average = _GetAverage(x, y);
          if ((fabs(average[0]-radiance[0])>std::max(2.0,radiance[0]*0.05)) ||
            (fabs(average[1]-radiance[1])>std::max(2.0,radiance[1]*0.05)) ||
            (fabs(average[2]-radiance[2])>std::max(2.0,radiance[2]*0.05)))
            {
            TS_FAIL("Wrong radiance returned.");
            break;
            }
          }
        }

      // Bounded rays should have no radiance.
      Spectrum_d black = mp_light->Radiance(RayDifferential(Ray(Point3D_d(), Vector3D_d(1,2,3).Normalized(), 0.0, 1.0)));
      TS_ASSERT(black.IsBlack());
      }

    void test_ImageEnvironmentalLight_Power()
      {
      double dx = fabs(m_bbox.m_max[0]-m_bbox.m_min[0]), dy = fabs(m_bbox.m_max[1]-m_bbox.m_min[1]), dz = fabs(m_bbox.m_max[2]-m_bbox.m_min[2]);

      Spectrum_d power;
      double d_phi = 2*M_PI/m_width;
      for(size_t y=0;y<m_height;++y)
        {
        double d_cos_theta = cos(y*M_PI/m_height) - cos((y+1)*M_PI/m_height);
        for(size_t x=0;x<m_width;++x)
          {
          double pixel_solid_angle = d_phi*d_cos_theta;
          Vector3D_d dir = MathRoutines::SphericalDirection<double>((x+0.5)*2*M_PI/m_width, (y+0.5)*M_PI/m_height);
          m_light_to_world(dir,dir);
          power += (dx*dy*fabs(dir[2])+dx*dz*fabs(dir[1])+dy*dz*fabs(dir[0]))*pixel_solid_angle*Convert<double>(m_image[y][x]);
          }
        }

      Spectrum_d power2 = mp_light->Power();
      TS_ASSERT_DELTA(power[0],power2[0],power[0]*0.05);
      TS_ASSERT_DELTA(power[1],power2[1],power[1]*0.05);
      TS_ASSERT_DELTA(power[2],power2[2],power[2]*0.05);
      }

    void test_ImageEnvironmentalLight_SampleLighting_WithoutNormal()
      {
      Spectrum_d radiance;
      double d_phi = 2*M_PI/m_width;
      for(size_t y=0;y<m_height;++y)
        {
        double d_cos_theta = cos(y*M_PI/m_height) - cos((y+1)*M_PI/m_height);
        for(size_t x=0;x<m_width;++x)
          radiance += (d_phi*d_cos_theta)*Convert<double>(m_image[y][x]);
        }

      size_t num_x = 256, num_y = 256;
      std::vector<Point2D_d> samples(num_x*num_y);
      SamplingRoutines::StratifiedSampling2D(samples.begin(), num_x, num_y, true);

      Spectrum_d radiance2;
      for(size_t i=0;i<samples.size();++i)
        {
        Vector3D_d dir;
        double pdf;
        Spectrum_d tmp = mp_light->SampleLighting(samples[i], dir, pdf);
        radiance2 += tmp/pdf;

        // We don't check PDFs near the poles because the Phi angle gets pretty distorted there.
        if (samples[i][1]>0.001 && samples[i][1]<0.999)
          {
          double pdf2 = mp_light->LightingPDF(dir);
          if (fabs(pdf-pdf2)>DBL_3D_EPS)
            {
            TS_FAIL("PDF value is incorrect.");
            return;
            }
          }
        }
      radiance2 /= samples.size();

      TS_ASSERT_DELTA(radiance[0],radiance2[0],radiance[0]*0.01);
      TS_ASSERT_DELTA(radiance[1],radiance2[1],radiance[1]*0.01);
      TS_ASSERT_DELTA(radiance[2],radiance2[2],radiance[2]*0.01);
      }

    void test_ImageEnvironmentalLight_LightingPDFSum_WithoutNormal()
      {
      double sum = 0.0;
      double d_phi = 2*M_PI/m_width;
      for(size_t y=0;y<m_height;++y)
        {
        double d_cos_theta = cos(y*M_PI/m_height) - cos((y+1)*M_PI/m_height);
        for(size_t x=0;x<m_width;++x)
          {
          Vector3D_d dir = MathRoutines::SphericalDirection<double>((x+0.5)*2*M_PI/m_width, (y+0.5)*M_PI/m_height);
          sum += (d_phi*d_cos_theta)*mp_light->LightingPDF(m_light_to_world(dir));
          }
        }

      TS_ASSERT_DELTA(sum, 1.0, (1e-7));
      }

    void test_ImageEnvironmentalLight_SampleLighting_WithNormal()
      {
      size_t N = 10;
      for(size_t t=0;t<N;++t)
        {
        Vector3D_d normal = Vector3D_d(RandomDouble(2.0)-1.0,RandomDouble(2.0)-1.0,RandomDouble(2.0)-1.0).Normalized();
        Spectrum_d irradiance = _ComputeIrradiance(normal);

        size_t num_x = 256, num_y = 256;
        std::vector<Point2D_d> samples(num_x*num_y);
        SamplingRoutines::StratifiedSampling2D(samples.begin(), num_x, num_y, true);

        Spectrum_d irradiance2;
        for(size_t i=0;i<samples.size();++i)
          {
          Vector3D_d dir;
          double pdf;
          Spectrum_d tmp = mp_light->SampleLighting(normal, samples[i], dir, pdf);
          irradiance2 += std::max(0.0,dir*normal)*tmp/pdf;

          // We don't check PDFs near the poles because the Phi angle gets pretty distorted there.
          if (samples[i][1]>0.001 && samples[i][1]<0.999)
            {
            bool matched = false;

            // Since the sampled direction can fall close to the edge of a texel we need to check nearest directions as well.
            // For that we try to displace the direction in all dimensions by a small value and if the PDFs do not match for *any* of the directions we report the error.
            for(char dx=-1;dx<=1 && matched==false;++dx) for(char dy=-1;dy<=1 && matched==false;++dy) for(char dz=-1;dz<=1 && matched==false;++dz)
              {
              double pdf2 = mp_light->LightingPDF(normal, Vector3D_d(dir[0]+dx*0.0001,dir[1]+dy*0.0001,dir[2]+dz*0.0001).Normalized());
              if (fabs(pdf-pdf2)<DBL_3D_EPS) matched=true;
              }
            if (matched == false)
              {
              TS_FAIL("PDF value is incorrect.");
              return;
              }
            }
          }
        irradiance2 /= samples.size();

        TS_ASSERT_DELTA(irradiance[0],irradiance2[0],irradiance[0]*0.01);
        TS_ASSERT_DELTA(irradiance[1],irradiance2[1],irradiance[1]*0.01);
        TS_ASSERT_DELTA(irradiance[2],irradiance2[2],irradiance[2]*0.01);
        }
      }

    void test_ImageEnvironmentalLight_LightingPDFSum_WithNormal()
      {
      size_t N = 10;
      for(size_t t=0;t<N;++t)
        {
        Vector3D_d normal = Vector3D_d(RandomDouble(2.0)-1.0,RandomDouble(2.0)-1.0,RandomDouble(2.0)-1.0).Normalized();

        double sum = 0.0;
        double d_phi = 2*M_PI/m_width;
        for(size_t y=0;y<m_height;++y)
          {
          double d_cos_theta = cos(y*M_PI/m_height) - cos((y+1)*M_PI/m_height);
          for(size_t x=0;x<m_width;++x)
            {
            Vector3D_d dir = MathRoutines::SphericalDirection<double>((x+0.5)*2*M_PI/m_width, (y+0.5)*M_PI/m_height);
            sum += (d_phi*d_cos_theta)*mp_light->LightingPDF(normal, m_light_to_world(dir));
            }
          }

        TS_ASSERT_DELTA(sum, 1.0, (1e-6));
        }
      }

    void test_ImageEnvironmentalLight_SamplePhoton()
      {
      size_t num_x = 256, num_y = 256;
      std::vector<Point2D_d> samples1(num_x*num_y);
      std::vector<Point2D_d> samples2(num_x*num_y);
      SamplingRoutines::StratifiedSampling2D(samples1.begin(), num_x, num_y, true);
      SamplingRoutines::StratifiedSampling2D(samples2.begin(), num_x, num_y, true);
      SamplingRoutines::Shuffle(samples2.begin(), samples2.size());

      Spectrum_d power;
      for(size_t i=0;i<samples1.size();++i)
        {
        Ray ray;
        double pdf;
        Spectrum_d sampled_irradiance = mp_light->SamplePhoton(samples1[i], samples2[i], ray, pdf);

        if (ray.m_direction.IsNormalized() == false)
          {
          TS_FAIL("The photon ray direction is not normalized.");
          break;
          }

        if (m_bbox.Inside( ray(ray.m_min_t) ))
          {
          TS_FAIL("The photon ray origin is in the bounding box.");
          break;
          }

        if (IsPositiveInf(ray.m_max_t)==false)
          {
          TS_FAIL("Photon ray is bounded.");
          break;
          }

        if (pdf <= 0.0)
          {
          TS_FAIL("PDF value is not positive.");
          break;
          }

        if (m_bbox.Intersect(ray))
          power += sampled_irradiance / pdf;
        }

      power /= samples2.size();

      Spectrum_d power2 = mp_light->Power();
      TS_ASSERT_DELTA(power[0],power2[0],power[0]*0.05);
      TS_ASSERT_DELTA(power[1],power2[1],power[1]*0.05);
      TS_ASSERT_DELTA(power[2],power2[2],power[2]*0.05);
      }

    void test_ImageEnvironmentalLight_Irradiance()
      {
      size_t N = 10;
      for(size_t t=0;t<N;++t)
        {
        Vector3D_d normal = Vector3D_d(RandomDouble(2.0)-1.0,RandomDouble(2.0)-1.0,RandomDouble(2.0)-1.0).Normalized();

        Spectrum_d irradiance = _ComputeIrradiance(normal);

        Spectrum_d irradiance2 = mp_light->Irradiance(normal);
        TS_ASSERT_DELTA(irradiance[0],irradiance2[0],irradiance[0]*0.15); // Such large error is due to the inaccuracy in irradiance computation in the environment light.
        TS_ASSERT_DELTA(irradiance[1],irradiance2[1],irradiance[1]*0.15); // With increasing MAX_TREE_DEPTH the error becomes smaller.
        TS_ASSERT_DELTA(irradiance[2],irradiance2[2],irradiance[2]*0.15);
        }
      }

    void test_ImageEnvironmentalLight_Fluence()
      {
      Spectrum_d irradiance;
      double d_phi = 2*M_PI/m_width;
      for(size_t y=0;y<m_height;++y)
        {
        double d_cos_theta = cos(y*M_PI/m_height) - cos((y+1)*M_PI/m_height);
        for(size_t x=0;x<m_width;++x)
          {
          double pixel_solid_angle = d_phi*d_cos_theta;
          Vector3D_d dir = MathRoutines::SphericalDirection<double>(x*2*M_PI/m_width, y*M_PI/m_height);
          m_light_to_world(dir,dir);
          irradiance += pixel_solid_angle*Convert<double>(m_image[y][x]);
          }
        }

      Spectrum_d irradiance2 = mp_light->Fluence();
      TS_ASSERT_DELTA(irradiance[0],irradiance2[0],irradiance[0]*0.01);
      TS_ASSERT_DELTA(irradiance[1],irradiance2[1],irradiance[1]*0.01);
      TS_ASSERT_DELTA(irradiance[2],irradiance2[2],irradiance[2]*0.01);
      }

    void test_ImageEnvironmentalLight_SmallMaps()
      {
      for(size_t x=1;x<=3;++x) for(size_t y=1;y<=3;++y)
        {
        std::vector<std::vector<Spectrum_f> > image(x, std::vector<Spectrum_f>(y, Spectrum_f(1.0)));

        m_bbox = BBox3D_d(Point3D_d(0,0,0), Point3D_d(10,20,30));
        m_light_to_world = MakeRotation(1.2, Vector3D_d(-1,2,-3).Normalized());      
        intrusive_ptr<InfiniteLightSource> p_light(new ImageEnvironmentalLight(m_bbox, m_light_to_world, image));

        size_t N=100;
        for(size_t i=0;i<N;++i)
          {
          Vector3D_d dir = Vector3D_d(RandomDouble(2.0)-1.0,RandomDouble(2.0)-1.0,RandomDouble(2.0)-1.0).Normalized();
          Spectrum_d radiance = p_light->Radiance(RayDifferential(Ray(Point3D_d(), dir)));
          CustomAssertDelta(radiance, Spectrum_d(1.0), (1e-6));
          }

        Spectrum_d irradiance = p_light->Fluence();
        CustomAssertDelta(irradiance, Spectrum_d(4.0*M_PI), (1e-8));
        }
      }

  private:

    Spectrum_d _GetAverage(double i_x, double i_y) const
      {
      double w = 0.0;
      Spectrum_d sum;

      for(int y=int(i_y)-1;y<=int(i_y)+1;++y)
        for(int x=int(i_x)-1;x<=int(i_x)+1;++x)
          {
          double tmp = exp(-(y-i_y)*(y-i_y)-(x-i_x)*(x-i_x));
          int x1 = (x+m_width)%m_width, y1=y;
          if (y1>=0 && y1<(int)m_height)
            {
            w += tmp;
            sum += tmp*Convert<double>(m_image[y1][x1]);
            }
          }

      return sum/w;
      }

    Spectrum_d _ComputeIrradiance(const Vector3D_d &i_normal) const
      {
      Spectrum_d irradiance;
      double d_phi = 2*M_PI/m_width;
      for(size_t y=0;y<m_height;++y)
        {
        double d_cos_theta = cos(y*M_PI/m_height) - cos((y+1)*M_PI/m_height);
        for(size_t x=0;x<m_width;++x)
          {
          double pixel_solid_angle = d_phi*d_cos_theta;
          Vector3D_d dir = MathRoutines::SphericalDirection<double>((x+0.5)*2*M_PI/m_width, (y+0.5)*M_PI/m_height);
          m_light_to_world(dir,dir);
          irradiance += std::max(dir*i_normal, 0.0) * pixel_solid_angle*Convert<double>(m_image[y][x]);
          }
        }

        return irradiance;
      }

  private:
    size_t m_width, m_height;
    std::vector<std::vector<Spectrum_f> > m_image;

    BBox3D_d m_bbox;
    intrusive_ptr<InfiniteLightSource> mp_light;
    Transform m_light_to_world;

  };

#endif // IMAGE_ENVIRONMENTAL_LIGHT_TEST_H
