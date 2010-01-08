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
      mp_reflectance = Spectrum_d(0.9,0.8,0.7);
      mp_transmittance = Spectrum_d(0.5,0.6,0.7);

      intrusive_ptr<Texture<Spectrum_d> > p_reflectance_texture( new TextureMock<Spectrum_d>(mp_reflectance) );
      intrusive_ptr<Texture<Spectrum_d> > p_transmittance_texture( new TextureMock<Spectrum_d>(mp_transmittance) );

      m_refractive_index=1.5;

      mp_material.reset(new Transparent(p_reflectance_texture, p_transmittance_texture, m_refractive_index));
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

      TS_ASSERT_EQUALS(p_bsdf->GetRefractiveIndex(), m_refractive_index);
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
      Spectrum_d f = p_bsdf->Sample(incident, exitant, Point2D_d(RandomDouble(1.0), RandomDouble(1.0)), RandomDouble(1.0), pdf, sampled_type, BSDF_ALL_REFLECTION);

      TS_ASSERT(sampled_type == BxDFType(BSDF_REFLECTION | BSDF_SPECULAR));
      TS_ASSERT(exitant == Vector3D_d(-incident[0],-incident[1],incident[2]));

      // Test that returned BSDF value is proportional to mp_reflectance.
      TS_ASSERT_DELTA(f[0]/mp_reflectance[0], f[1]/mp_reflectance[1], 1e-10);
      TS_ASSERT_DELTA(f[0]/mp_reflectance[0], f[2]/mp_reflectance[2], 1e-10);
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
      Spectrum_d f = p_bsdf->Sample(incident, exitant, Point2D_d(RandomDouble(1.0), RandomDouble(1.0)), RandomDouble(1.0), pdf, sampled_type, BSDF_ALL_TRANSMISSION);

      TS_ASSERT(sampled_type == BxDFType(BSDF_TRANSMISSION | BSDF_SPECULAR));
      TS_ASSERT(exitant[2]<0.0);

      // Test that returned BSDF value is proportional to mp_transmittance.
      TS_ASSERT_DELTA(f[0]/mp_transmittance[0], f[1]/mp_transmittance[1], 1e-10);
      TS_ASSERT_DELTA(f[0]/mp_transmittance[0], f[2]/mp_transmittance[2], 1e-10);
      }

  private:
    intrusive_ptr<Material> mp_material;
    Spectrum_d mp_reflectance, mp_transmittance;
    double m_refractive_index;
  };

#endif // TRANSPARENT_TEST_H
