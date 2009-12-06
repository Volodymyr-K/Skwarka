#ifndef TRANSPARENT_TEST_H
#define TRANSPARENT_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Common/MemoryPool.h>
#include <Raytracer/Core/Material.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/BSDF.h>
#include <Raytracer/Materials/Transparent.h>
#include "Mocks/TextureMock.h"
#include <Math/ThreadSafeRandom.h>

class TransparentTestSuite : public CxxTest::TestSuite
  {
  public:
    void setUp()
      {
      intrusive_ptr<Texture<Spectrum_d> > p_reflectance( new TextureMock<Spectrum_d>(Spectrum_d(1.0,1.0,1.0)) );
      intrusive_ptr<Texture<Spectrum_d> > p_transmittance( new TextureMock<Spectrum_d>(Spectrum_d(1.0,1.0,1.0)) );

      mp_material.reset(new Transparent(p_reflectance, p_transmittance, 1.5));
      }

    void tearDown()
      {
      // Nothing to clear.
      }

    // Tests that the material returns a BSDF with one reflective specular and one reflective transmitting component.
    void test_Transparent_Components()
      {
      MemoryPool pool;
      DifferentialGeometry dg;
      dg.m_geometric_normal=dg.m_shading_normal=Vector3D_d(0.0,0.0,1.0);
      const BSDF *p_bsdf = mp_material->GetBSDF(dg, 0, pool);

      TS_ASSERT(p_bsdf != NULL);
      TS_ASSERT_EQUALS(p_bsdf->GetComponentsNum(), 2);
      TS_ASSERT_EQUALS(p_bsdf->GetComponentsNum(BxDFType(BSDF_REFLECTION | BSDF_SPECULAR)), 1);
      TS_ASSERT_EQUALS(p_bsdf->GetComponentsNum(BxDFType(BSDF_TRANSMISSION | BSDF_SPECULAR)), 1);
      }

    void test_Transparent_RefractiveIndex()
      {
      MemoryPool pool;
      DifferentialGeometry dg;
      dg.m_geometric_normal=dg.m_shading_normal=Vector3D_d(0.0,0.0,1.0);
      const BSDF *p_bsdf = mp_material->GetBSDF(dg, 0, pool);

      TS_ASSERT_EQUALS(p_bsdf->GetRefractiveIndex(), 1.5);
      }

    void test_Transparent_SpecularReflection()
      {
      MemoryPool pool;
      DifferentialGeometry dg;
      dg.m_geometric_normal=dg.m_shading_normal=Vector3D_d(0.0,0.0,1.0);
      const BSDF *p_bsdf = mp_material->GetBSDF(dg, 0, pool);

      Vector3D_d incident=Vector3D_d(0.5,0.5,0.5).Normalized();
      Vector3D_d exitant;
      double pdf;
      BxDFType sampled_type;
      p_bsdf->Sample(incident, exitant, Point2D_d(RandomDouble(1.0), RandomDouble(1.0)), RandomDouble(1.0), pdf, sampled_type, BSDF_ALL_REFLECTION);

      TS_ASSERT(sampled_type == BxDFType(BSDF_REFLECTION | BSDF_SPECULAR));
      TS_ASSERT(exitant == Vector3D_d(-incident[0],-incident[1],incident[2]));
      }

    void test_Transparent_SpecularTransmittance()
      {
      MemoryPool pool;
      DifferentialGeometry dg;
      dg.m_geometric_normal=dg.m_shading_normal=Vector3D_d(0.0,0.0,1.0);
      const BSDF *p_bsdf = mp_material->GetBSDF(dg, 0, pool);

      Vector3D_d incident=Vector3D_d(0.5,0.5,0.5).Normalized();
      Vector3D_d exitant;
      double pdf;
      BxDFType sampled_type;
      p_bsdf->Sample(incident, exitant, Point2D_d(RandomDouble(1.0), RandomDouble(1.0)), RandomDouble(1.0), pdf, sampled_type, BSDF_ALL_TRANSMISSION);

      TS_ASSERT(sampled_type == BxDFType(BSDF_TRANSMISSION | BSDF_SPECULAR));
      TS_ASSERT(exitant[2]<0.0);
      }

  private:
    intrusive_ptr<Material> mp_material;
  };

#endif // TRANSPARENT_TEST_H
