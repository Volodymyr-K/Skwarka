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

#ifndef UBER_MATERIAL_TEST_H
#define UBER_MATERIAL_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/MemoryPool.h>
#include <Raytracer/Core/Material.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/BSDF.h>
#include <Raytracer/Materials/UberMaterial.h>
#include "Mocks/TextureMock.h"

class UberMaterialTestSuite : public CxxTest::TestSuite
  {
  public:

    // Just evaluate the BSDF with some random parameters, no special check here.
    void test_UberMaterial()
      {
      intrusive_ptr<Texture<SpectrumCoef_d>> p_Kd( new TextureMock<SpectrumCoef_d>(SpectrumCoef_d(0.1,0.2,0.3)) );
      intrusive_ptr<Texture<SpectrumCoef_d>> p_Ks( new TextureMock<SpectrumCoef_d>(SpectrumCoef_d(0.3,0.1,0.05)) );
      intrusive_ptr<Texture<SpectrumCoef_d>> p_Kr( new TextureMock<SpectrumCoef_d>(SpectrumCoef_d(0.15,0.12,0.25)) );
      intrusive_ptr<Texture<SpectrumCoef_d>> p_opacity( new TextureMock<SpectrumCoef_d>(SpectrumCoef_d(0.2,0.2,0.2)) );
      intrusive_ptr<Texture<double>> p_roughness( new TextureMock<double>(0.1) );

      intrusive_ptr<Material> p_material(new UberMaterial(p_Kd, p_Ks, p_Kr, p_roughness, p_opacity, 1.5));

      MemoryPool pool;
      DifferentialGeometry dg;
      dg.m_geometric_normal=dg.m_shading_normal=Vector3D_d(0.0,0.0,1.0);
      dg.m_tangent=Vector3D_d(1.0,0.0,0.0);
      const BSDF *p_bsdf = p_material->GetBSDF(dg, 0, pool);
      TS_ASSERT(p_bsdf != NULL);
      }

  };

#endif // UBER_MATERIAL_TEST_H
