#ifndef MIP_MAP_TEST_H
#define MIP_MAP_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/Core/MIPMap.h>
#include <Raytracer/Core/Spectrum.h>
#include <vector>

class MIPMapTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_MIPMap_Trilinear1()
      {
      intrusive_ptr<MIPMap<double>> p_map = _CreateWhiteMIPMap(11,17,1.0,true,8.0);

      double t = p_map->Evaluate(Point2D_d(0.5,0.5),0.1);
      TS_ASSERT_DELTA(t, 1.0,1e-10);
      }

    void test_MIPMap_Trilinear2()
      {
      intrusive_ptr<MIPMap<double>> p_map = _CreateWhiteMIPMap(11,17,1.0,true,8.0);

      double t = p_map->Evaluate(Point2D_d(2.5,0.5),0.1);
      TS_ASSERT_DELTA(t, 1.0,1e-10);
      }

    void test_MIPMap_Trilinear3()
      {
      intrusive_ptr<MIPMap<double>> p_map = _CreateWhiteMIPMap(11,17,1.0,false,8.0);

      double t = p_map->Evaluate(Point2D_d(2.5,0.5),0.1);
      TS_ASSERT_DELTA(t, 0.0,1e-10);
      }

    void test_MIPMap_Trilinear4()
      {
      intrusive_ptr<MIPMap<double>> p_map = _CreateWhiteMIPMap(11,17,1.0,false,8.0);

      double t = p_map->Evaluate(Point2D_d(1.0,1.0),0.1);
      TS_ASSERT_DELTA(t, 0.25, 1e-10);
      }

    // Tests different orientations of ellipse near the border of different colors.
    void test_MIPMap_Anisotropic1()
      {
      intrusive_ptr<MIPMap<Spectrum_d>> p_map = _CreateBlueRedMIPMap(40,60,false,8.0);

      Spectrum_d t1 = p_map->Evaluate(Point2D_d(0.47,0.2), Vector2D_d(0.02,0.0), Vector2D_d(0.0,0.2));
      Spectrum_d t2 = p_map->Evaluate(Point2D_d(0.47,0.2), Vector2D_d(0.2,0.0), Vector2D_d(0.0,0.02));

      TS_ASSERT_DELTA(t1[0]+t1[1]+t1[2],1.0,1e-10);
      TS_ASSERT(t1[0]>t1[2] && t1[2]<0.1);

      TS_ASSERT_DELTA(t2[0]+t2[1]+t2[2],1.0,1e-10);
      TS_ASSERT(t2[0]>t2[2] && t2[2]>0.1);
      }

    void test_MIPMap_Anisotropic2()
      {
      intrusive_ptr<MIPMap<Spectrum_d>> p_map = _CreateBlueRedMIPMap(40,60,false,8.0);

      Spectrum_d t = p_map->Evaluate(Point2D_d(2.0,-0.2), Vector2D_d(0.02,0.0), Vector2D_d(0.0,0.2));
      TS_ASSERT(t.IsBlack());
      }

    // Tests on a 1x1 image size.
    void test_MIPMap_Anisotropic3()
      {
      intrusive_ptr<MIPMap<Spectrum_d>> p_map = _CreateBlueRedMIPMap(1,1,true,8.0);

      Spectrum_d t = p_map->Evaluate(Point2D_d(0.3,0.7), Vector2D_d(0.02,0.0), Vector2D_d(0.0,0.2));
      TS_ASSERT_EQUALS(t, Spectrum_d(0.0,0.0,1.0));
      }


  private:
    intrusive_ptr<MIPMap<double>> _CreateWhiteMIPMap(size_t i_width, size_t i_height, double i_value, bool i_repeat, double i_max_anisotropy)
      {
      std::vector<std::vector<double>> image(i_height,std::vector<double>(i_width,i_value));

      return intrusive_ptr<MIPMap<double>> (new MIPMap<double>(image,i_repeat,i_max_anisotropy));
      }

    intrusive_ptr<MIPMap<Spectrum_d>> _CreateBlueRedMIPMap(size_t i_width, size_t i_height, bool i_repeat, double i_max_anisotropy)
      {
      std::vector<std::vector<Spectrum_d>> image(i_height,std::vector<Spectrum_d>(i_width));

      for(size_t i=0;i<i_width;++i)
        for(size_t j=0;j<i_height;++j)
          if (i<i_width/2)
            image[j][i]=Spectrum_d(1.0,0.0,0.0);
          else
            image[j][i]=Spectrum_d(0.0,0.0,1.0);

      return intrusive_ptr<MIPMap<Spectrum_d>> (new MIPMap<Spectrum_d>(image,i_repeat,i_max_anisotropy));
      }
  };

#endif // MIP_MAP_TEST_H
