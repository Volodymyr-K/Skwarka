#ifndef WINDY_TEXTURE_TEST_H
#define WINDY_TEXTURE_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/Textures/WindyTexture.h>
#include <Raytracer/Mappings/TransformMapping3D.h>
#include <Math/ThreadSafeRandom.h>
#include <Raytracer/Core/DifferentialGeometry.h>

class WindyTextureTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_WindyTexture_Range()
      {
      intrusive_ptr<const Mapping3D> p_mapping(new TransformMapping3D());
      intrusive_ptr<Texture<double>> p_texture(new WindyTexture<double>(p_mapping));

      double mn=DBL_INF, mx=-DBL_INF;
      for (size_t i=0; i<10000; ++i)
        {
        DifferentialGeometry dg;
        dg.m_point = Point3D_d(RandomDouble(100.0)-50.0, RandomDouble(100.0)-50.0, RandomDouble(100.0)-50.0);
        double val = p_texture->Evaluate(dg, 0);
        if (val>mx) mx=val;
        if (val<mn) mn=val;
        }

      ASSERT(mn>=-1.0 && mn<=1.0);
      ASSERT(mx>=-1.0 && mx<=1.0);
      }
  };

#endif // WINDY_TEXTURE_TEST_H
