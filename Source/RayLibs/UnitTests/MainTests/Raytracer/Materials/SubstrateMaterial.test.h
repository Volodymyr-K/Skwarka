/*
* Copyright (C) 2014 by Volodymyr Kachurovskyi <Volodymyr.Kachurovskyi@gmail.com>
*
* This file is part of Skwarka.
*
* Skwarka is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
*
* Skwarka is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with Skwarka.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SUBSTRATE_MATERIAL_TEST_H
#define SUBSTRATE_MATERIAL_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/MemoryPool.h>
#include <Raytracer/Core/Material.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/BSDF.h>
#include <Raytracer/Materials/SubstrateMaterial.h>
#include <Raytracer/BxDFs/FresnelBlend.h>
#include <Raytracer/MicrofacetDistributions/BlinnDistribution.h>
#include <Raytracer/MicrofacetDistributions/AnisotropicDistribution.h>
#include <Raytracer/Core/Fresnel.h>
#include "Mocks/TextureMock.h"

class SubstrateMaterialTestSuite : public CxxTest::TestSuite
  {
  public:

    void test_SubstrateMaterial_Isotropic()
      {
      SpectrumCoef_d diffuse_reflectance(0.4,0.5,0.6), specular_reflectance(0.5);
      intrusive_ptr<Texture<SpectrumCoef_d>> p_diffuse_reflectance( new TextureMock<SpectrumCoef_d>(diffuse_reflectance) );
      intrusive_ptr<Texture<SpectrumCoef_d>> p_specular_reflectance( new TextureMock<SpectrumCoef_d>(specular_reflectance) );
      intrusive_ptr<Texture<double>> p_roughness( new TextureMock<double>(0.025) );

      intrusive_ptr<Material> p_material(new SubstrateMaterial(p_diffuse_reflectance, p_specular_reflectance, p_roughness));

      MemoryPool pool;
      DifferentialGeometry dg;
      dg.m_geometric_normal=dg.m_shading_normal=Vector3D_d(0.0,0.0,1.0);
      dg.m_tangent=Vector3D_d(1.0,0.0,0.0);
      const BSDF *p_bsdf = p_material->GetBSDF(dg, 0, pool);

      TS_ASSERT(p_bsdf != NULL);
      TS_ASSERT_EQUALS(p_bsdf->GetComponentsNum(), 1);

      SpectrumCoef_d val = p_bsdf->Evaluate(Vector3D_d(0.5,0.0,1.0).Normalized(), Vector3D_d(-0.5,0.0,1.0).Normalized());

      typedef FresnelBlend<BlinnDistribution> BlinnFresnelBlend;
      BlinnFresnelBlend fresnel_blend(diffuse_reflectance, specular_reflectance, BlinnDistribution(1.0/0.025));
      SpectrumCoef_d val2 = fresnel_blend.Evaluate(Vector3D_d(0.5,0.0,1.0).Normalized(), Vector3D_d(-0.5,0.0,1.0).Normalized());

      CustomAssertDelta(val, val2, (1e-6));
      }

    void test_SubstrateMaterial_Anisotropic()
      {
      SpectrumCoef_d diffuse_reflectance(0.4,0.5,0.6), specular_reflectance(0.5);
      intrusive_ptr<Texture<SpectrumCoef_d>> p_diffuse_reflectance( new TextureMock<SpectrumCoef_d>(diffuse_reflectance) );
      intrusive_ptr<Texture<SpectrumCoef_d>> p_specular_reflectance( new TextureMock<SpectrumCoef_d>(specular_reflectance) );
      intrusive_ptr<Texture<double>> p_u_roughness( new TextureMock<double>(0.025) );
      intrusive_ptr<Texture<double>> p_v_roughness( new TextureMock<double>(0.125) );

      intrusive_ptr<Material> p_material(new SubstrateMaterial(p_diffuse_reflectance, p_specular_reflectance, p_u_roughness, p_v_roughness));

      MemoryPool pool;
      DifferentialGeometry dg;
      dg.m_geometric_normal=dg.m_shading_normal=Vector3D_d(0.0,0.0,1.0);
      dg.m_tangent=Vector3D_d(1.0,0.0,0.0);
      const BSDF *p_bsdf = p_material->GetBSDF(dg, 0, pool);

      TS_ASSERT(p_bsdf != NULL);
      TS_ASSERT_EQUALS(p_bsdf->GetComponentsNum(), 1);

      SpectrumCoef_d val = p_bsdf->Evaluate(Vector3D_d(0.5,0.0,1.0).Normalized(), Vector3D_d(-0.5,0.0,1.0).Normalized());

      typedef FresnelBlend<AnisotropicDistribution> AnisotropicFresnelBlend;
      AnisotropicFresnelBlend fresnel_blend(diffuse_reflectance, specular_reflectance, AnisotropicDistribution(1.0/0.025, 1.0/0.125));
      SpectrumCoef_d val2 = fresnel_blend.Evaluate(Vector3D_d(0.5,0.0,1.0).Normalized(), Vector3D_d(-0.5,0.0,1.0).Normalized());

      CustomAssertDelta(val, val2, (1e-6));
      }

  };

#endif // SUBSTRATE_MATERIAL_TEST_H
