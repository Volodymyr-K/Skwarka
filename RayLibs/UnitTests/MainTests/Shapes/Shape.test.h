#ifndef SHAPE_TEST_H
#define SHAPE_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Shapes/Shape.h>
#include <Raytracer/Core/TriangleMesh.h>
#include "Mocks/ShapeMock.h"
#include <Raytracer/Core/Spectrum.h>
#include <string>

class ShapeTestSuite : public CxxTest::TestSuite
  {
  public:
    
    void test_Shape_ReadCorrectParameter()
      {
      ShapeMock shape;
      shape.SetParameter("Color","0.3 0.4 -0.1");

      Spectrum_d spectrum;
      // We intentionally mix lowercase and uppercase letters to test that the parameter name is case-insensitive.
      bool read_succesfully = shape.GetParameter("coLor",spectrum,true);

      TS_ASSERT_EQUALS(spectrum, Spectrum_d(0.3, 0.4, -0.1));
      TS_ASSERT(read_succesfully);
      }

    void test_Shape_ReadOverwrittenParameter()
      {
      ShapeMock shape;
      shape.SetParameter("Color","0.3 0.4 -0.1");
      shape.SetParameter("color","-0.3 -0.4 0.1");

      Spectrum_d spectrum;
      bool read_succesfully = shape.GetParameter("color",spectrum,true);

      TS_ASSERT_EQUALS(spectrum, Spectrum_d(-0.3, -0.4, 0.1));
      TS_ASSERT(read_succesfully);
      }

    void test_Shape_ReadIncorrectParameter()
      {
      ShapeMock shape;
      shape.SetParameter("Color","abracadabra");

      Spectrum_d spectrum;
      bool read_succesfully = shape.GetParameter("color",spectrum,true);

      TS_ASSERT(read_succesfully==false);
      }

    void test_Shape_ReadMissingParameter()
      {
      ShapeMock shape;
      shape.SetParameter("other parameter","abracadabra");

      Spectrum_d spectrum;
      bool read_succesfully = shape.GetParameter("color",spectrum,true);

      TS_ASSERT(read_succesfully==false);
      }

    void test_Shape_AddError()
      {
      ShapeMock shape;

      shape.AddError("my error");
      TS_ASSERT(shape.ErrorsExist());
      TS_ASSERT(shape.GetErrors()[0]=="my error");
      }

    void test_Shape_ClearErrors()
      {
      ShapeMock shape;

      shape.AddError("my error");
      shape.ClearErrors();
      TS_ASSERT(shape.ErrorsExist()==false);
      }

    // Test for error when a required parameter is missing.
    void test_Shape_MissingParameterError1()
      {
      ShapeMock shape;

      Spectrum_d spectrum;
      bool read_succesfully = shape.GetParameter("color",spectrum,true);

      TS_ASSERT(shape.ErrorsExist());
      }

    // Test for error when an optional parameter is missing.
    void test_Shape_MissingParameterError2()
      {
      ShapeMock shape;

      Spectrum_d spectrum;
      bool read_succesfully = shape.GetParameter("color",spectrum,false);

      TS_ASSERT(shape.ErrorsExist()==false);
      }
  };

#endif // SHAPE_TEST_H
