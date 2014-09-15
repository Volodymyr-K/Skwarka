#ifndef BLOCKED_ARRAY_TEST_H
#define BLOCKED_ARRAY_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Math/BlockedArray.h>
#include <Raytracer/Core/Spectrum.h>

class BlockedArrayTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_BlockedArray_ConstrFromData()
      {
      size_t n=55,m=122;
      std::vector<std::vector<Spectrum_d> > spectrums(n, std::vector<Spectrum_d>(m));
      for(size_t i=0;i<n;++i)
        for(size_t j=0;j<m;++j)
          spectrums[i][j]=Spectrum_d(i*3+j);

      BlockedArray<Spectrum_d> A(spectrums);
      for(size_t i=0;i<n;++i)
        for(size_t j=0;j<m;++j)
          if (A.Get(i,j)!=spectrums[i][j])
            {
            TS_FAIL("Values do not coincide.");
            return;
            }
      }

    void test_BlockedArray_Size()
      {
      size_t n=55,m=122;
      BlockedArray<double,3> A(n,m);

      TS_ASSERT_EQUALS(A.GetSizeU(), n);
      TS_ASSERT_EQUALS(A.GetSizeV(), m);
      }

    void test_BlockedArray_SetGet()
      {
      size_t n=55,m=122;
      BlockedArray<double,3> A(n,m);
      for(size_t i=0;i<n;++i)
        for(size_t j=0;j<m;++j)
          A.Get(i,j)=3*i+j;

      for(size_t i=0;i<n;++i)
        for(size_t j=0;j<m;++j)
          if (A.Get(i,j)!=3*i+j)
            {
            TS_FAIL("Values do not coincide.");
            return;
            }
      }

    void test_BlockedArray_Fill()
      {
      size_t n=55,m=122;
      BlockedArray<double,3> A(n,m);
      for(size_t i=0;i<n;++i)
        for(size_t j=0;j<m;++j)
          A.Get(i,j)=3*i+j;

      A.Fill(-1.5);

      for(size_t i=0;i<n;++i)
        for(size_t j=0;j<m;++j)
          if (A.Get(i,j)!=-1.5)
            {
            TS_FAIL("Values do not coincide.");
            return;
            }
      }
  };

#endif // BLOCKED_ARRAY_TEST_H
