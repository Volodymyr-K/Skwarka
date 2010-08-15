#ifndef COLOR_TEST_H
#define COLOR_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/Core/Color.h>

class ColorTestSuite : public CxxTest::TestSuite
  {
  public:

    void test_ColorSystem_RGB_To_XYZ()
      {
      ColorSystem cs(Point2D_d(0.7,0.3), Point2D_d(0.1,0.8), Point2D_d(0.1,0.1), Point2D_d(0.4, 0.3));

      // Black.
      XYZColor_f xyz = cs.RGB_To_XYZ(RGBColor_f(0,0,0));
      TS_ASSERT_EQUALS(xyz[0], 0.f);
      TS_ASSERT_EQUALS(xyz[1], 0.f);
      TS_ASSERT_EQUALS(xyz[2], 0.f);

      // White point.
      xyz = cs.RGB_To_XYZ(RGBColor_f(1,1,1));
      TS_ASSERT_DELTA(xyz[0]/(xyz[0]+xyz[1]+xyz[2]), 0.4f, (1e-5));
      TS_ASSERT_DELTA(xyz[1]/(xyz[0]+xyz[1]+xyz[2]), 0.3f, (1e-5));
      TS_ASSERT_DELTA(xyz[1], 1.0, (1e-5));

      // First primary color.
      xyz = cs.RGB_To_XYZ(RGBColor_f(1,0,0));
      TS_ASSERT_DELTA(xyz[0]/(xyz[0]+xyz[1]+xyz[2]), 0.7f, (1e-5));
      TS_ASSERT_DELTA(xyz[1]/(xyz[0]+xyz[1]+xyz[2]), 0.3f, (1e-5));

      // Second primary color.
      xyz = cs.RGB_To_XYZ(RGBColor_f(0,1,0));
      TS_ASSERT_DELTA(xyz[0]/(xyz[0]+xyz[1]+xyz[2]), 0.1f, (1e-5));
      TS_ASSERT_DELTA(xyz[1]/(xyz[0]+xyz[1]+xyz[2]), 0.8f, (1e-5));

      // Third primary color.
      xyz = cs.RGB_To_XYZ(RGBColor_f(0,0,1));
      TS_ASSERT_DELTA(xyz[0]/(xyz[0]+xyz[1]+xyz[2]), 0.1f, (1e-5));
      TS_ASSERT_DELTA(xyz[1]/(xyz[0]+xyz[1]+xyz[2]), 0.1f, (1e-5));
      }

    void test_ColorSystem_XYZ_To_RGB()
      {
      ColorSystem cs(Point2D_d(0.4,0.5), Point2D_d(0.1,0.5), Point2D_d(0.3,0.1), Point2D_d(0.3, 0.2));

      RGBColor_d rgb = cs.XYZ_To_RGB(XYZColor_d(0.3,0.6,0.1), false); // This point lies outside of the RGB triangle.
      TS_ASSERT(rgb[2] < 0.0);

      XYZColor_d xyz = cs.RGB_To_XYZ(rgb);
      TS_ASSERT_DELTA(xyz[0], 0.3, (1e-10));
      TS_ASSERT_DELTA(xyz[1], 0.6, (1e-10));
      TS_ASSERT_DELTA(xyz[2], 0.1, (1e-10));

      rgb = cs.XYZ_To_RGB(XYZColor_d(0.3,0.6,0.1), true);
      TS_ASSERT(fabs(rgb[2]) < DBL_EPS); // The constrained point should lie on the opposite triangle's edge.
      
      xyz = cs.RGB_To_XYZ(rgb);
      TS_ASSERT_DELTA(xyz[1], 0.6, (1e-10)); // The luminance of the constrained point should be the same.
      }

  };

#endif // COLOR_TEST_H