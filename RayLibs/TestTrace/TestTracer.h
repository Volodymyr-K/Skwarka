#ifndef TESTTRACER_H
#define TESTTRACER_H

#pragma warning(disable : 4003)
#include <Common/Common.h>

#include "tbb/pipeline.h"
#include "tbb/tick_count.h"
#include "tbb/task_scheduler_init.h"
#include <sstream>

//#include "vld.h"

#include "MeshLoader.h"
#include <Math/Geometry.h>
#include <Raytracer/Core/TriangleMesh.h>
#include <Shapes/Sphere.h>
#include "stdafx.h"
#include <WinBase.h>
#include <cstdio>
#include <Raytracer/Core/Spectrum.h>
#include <Math/MathRoutines.h>
#include <Raytracer/Core/Camera.h>
#include <Raytracer/FilmFilters/BoxFilter.h>
#include <Raytracer/FilmFilters/MitchellFilter.h>
#include <Raytracer/Core/Sample.h>
#include <Raytracer/Samplers/RandomSampler.h>
#include <Raytracer/Samplers/StratifiedSampler.h>
#include <Raytracer/Samplers/LDSampler.h>
#include <Raytracer/Cameras/PerspectiveCamera.h>
#include <Math/ThreadSafeRandom.h>
#include <Raytracer/Samplers/UniformImagePixelsOrder.h>
#include <Raytracer/Samplers/ConsecutiveImagePixelsOrder.h>
#include <Raytracer/Samplers/RandomBlockedImagePixelsOrder.h>
#include <Raytracer/Films/ImageFilm.h>
#include <Raytracer/Films/InteractiveFilm.h>
#include <Raytracer/Materials/MatteMaterial.h>
#include <Raytracer/Textures/ConstantTexture.h>
#include <Raytracer/Renderers/SamplerBasedRenderer.h>
#include <Raytracer/LightSources/PointLight.h>
#include <Raytracer/LightSources/SpotPointLight.h>
#include <Raytracer/LightSources/ParallelLight.h>
#include <Raytracer/LightSources/DiffuseAreaLightSource.h>
#include <Raytracer/LTEIntegrators/DirectLightingLTEIntegrator.h>
#include <Raytracer/LTEIntegrators/PhotonLTEIntegrator.h>
#include <UnitTests/Mocks/InfiniteLightSourceMock.h>
#include <Raytracer/LightsSamplingStrategies/IrradianceLightsSamplingStrategy.h>
#include <Raytracer/LightsSamplingStrategies/PowerLightsSamplingStrategy.h>
#include <Raytracer/Core/MIPMap.h>
#include <Raytracer/Textures/ImageTexture.h>
#include <Raytracer/Mappings/SphericalMapping2D.h>
#include <Raytracer/Mappings/UVMapping2D.h>
#include <Raytracer/Materials/TransparentMaterial.h>
#include <Raytracer/Materials/MetalMaterial.h>
#include <Raytracer/Core/Fresnel.h>
#include <Math/CompressedDirection.h>
#include <Raytracer/Materials/SubstrateMaterial.h>
#include <Raytracer/Materials/PlasticMaterial.h>
#include <Raytracer/BxDFs/FresnelBlend.h>
#include <Raytracer/MicrofacetDistributions/AnisotropicDistribution.h>
#include <Raytracer/MicrofacetDistributions/BlinnDistribution.h>
#include "EasyBMP.h"
#include <Raytracer/LightSources/ImageEnvironmentalLight.h>
#include <Raytracer/VolumeRegions/HomogeneousVolumeRegion.h>
#include <Raytracer/VolumeRegions/GridDensityVolumeRegion.h>
#include <Raytracer/PhaseFunctions/IsotropicPhaseFunction.h>
#include <Raytracer/PhaseFunctions/MieHazyPhaseFunction.h>
#include <Raytracer/PhaseFunctions/MieMurkyPhaseFunction.h>
#include <Raytracer/PhaseFunctions/RayleighPhaseFunction.h>
#include <Raytracer/ImageSources/RGB24ImageSource.h>
#include <Raytracer/ImageSources/OpenEXRRgbaImageSource.h>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>

#include <ImfRgbaFile.h>
#include <ImfStringAttribute.h>
#include <ImfMatrixAttribute.h>
#include <ImfArray.h>

class TestTracer
  {
  public:
    TestTracer(int w, int h)
      {
      m_imageWidth=w;
      m_imageHeight=h;
      }

    void LoadMesh();

    void SetDisplayUpdateCallback(DisplayUpdateCallback *ip_callback)
      {
      mp_callback=ip_callback;
      }

    int GetImageWidth() const { return m_imageWidth; }
    int GetImageHeight() const { return m_imageHeight; }

    void RenderImage();

  public:
  int     m_imageWidth;
  int     m_imageHeight;

  DisplayUpdateCallback *mp_callback;
  intrusive_ptr<TriangleMesh> mp_mesh;
  intrusive_ptr<Scene> mp_scene;
  };

intrusive_ptr<Primitive> LoadWallsPrimitive(std::string i_filename, bool i_smooth)
  {
  intrusive_ptr<TriangleMesh> p_mesh( LoadMeshFromStl(i_filename, i_smooth) );

  BMP Input;
  Input.ReadFromFile("walls.bmp");

  std::vector<std::vector<RGB24> > values(Input.TellHeight(),std::vector<RGB24>(Input.TellWidth()));
  for( int j=0 ; j < Input.TellHeight() ; j++)
    {
    for( int i=0 ; i < Input.TellWidth() ; i++)
      {
      RGB24 rgb;
      rgb.m_rgb[0]=Input(i,j)->Red;
      rgb.m_rgb[1]=Input(i,j)->Green;
      rgb.m_rgb[2]=Input(i,j)->Blue;
      values[j][i]=rgb;
      }
    }
  intrusive_ptr<ImageSource<SpectrumCoef_f> > p_image_source( new RGB24ImageSource<SpectrumCoef_f>(values, global_sRGB_E_ColorSystem, 0.8) );

  intrusive_ptr<Mapping2D> p_mapping( new SphericalMapping2D(Point3D_d(-770,-2315,1500), Vector3D_d(0,0,1), Vector3D_d(1,0,0)) );
  intrusive_ptr< ImageTexture<SpectrumCoef_f,SpectrumCoef_d> > p_text(new ImageTexture<SpectrumCoef_f,SpectrumCoef_d>(p_image_source, p_mapping) );

  intrusive_ptr<Texture<double> > p_sigma(new ConstantTexture<double> (0.04));
  intrusive_ptr<Material> p_material(new MatteMaterial(p_text, p_sigma));

  intrusive_ptr<Primitive> p_primitive(new Primitive(p_mesh, p_material));
  return p_primitive;
  }

intrusive_ptr<Primitive> LoadDiffusePrimitive(std::string i_filename, bool i_smooth, SpectrumCoef_d i_color)
  {
  intrusive_ptr<TriangleMesh> p_mesh( LoadMeshFromStl(i_filename, i_smooth) );

  intrusive_ptr<Texture<SpectrumCoef_d> > p_reflectance(new ConstantTexture<SpectrumCoef_d> (i_color));
  intrusive_ptr<Texture<double> > p_sigma(new ConstantTexture<double> (0.04));
  intrusive_ptr<Material> p_material(new MatteMaterial(p_reflectance, p_sigma));

  intrusive_ptr<Primitive> p_primitive(new Primitive(p_mesh, p_material));
  return p_primitive;
  }

intrusive_ptr<Primitive> LoadGlassPrimitive(std::string i_filename, bool i_smooth, SpectrumCoef_d i_color)
  {
  intrusive_ptr<TriangleMesh> p_mesh( LoadMeshFromStl(i_filename, i_smooth) );

  intrusive_ptr<Texture<SpectrumCoef_d> > p_reflectance(new ConstantTexture<SpectrumCoef_d> (SpectrumCoef_d(1.0)));
  intrusive_ptr<Texture<SpectrumCoef_d> > p_transmittance(new ConstantTexture<SpectrumCoef_d> (i_color));
  intrusive_ptr<Material> p_material(new TransparentMaterial(p_reflectance, p_transmittance, 1.4));

  intrusive_ptr<Primitive> p_primitive(new Primitive(p_mesh, p_material));
  return p_primitive;
  }

intrusive_ptr<Primitive> LoadMetalPrimitive(std::string i_filename, bool i_smooth, SpectrumCoef_d i_color, double i_roughness)
  {
  intrusive_ptr<TriangleMesh> p_mesh( LoadMeshFromStl(i_filename, i_smooth) );

  intrusive_ptr<Texture<SpectrumCoef_d> > p_refrlection(new ConstantTexture<SpectrumCoef_d> (i_color));
  intrusive_ptr<Texture<double> > p_roughness(new ConstantTexture<double> (i_roughness));
  intrusive_ptr<Material> p_material(new MetalMaterial(p_refrlection, p_roughness));

  intrusive_ptr<Primitive> p_primitive(new Primitive(p_mesh, p_material));
  return p_primitive;
  }

void
readRgba1 (const char fileName[],
           Imf::Array2D<Imf::Rgba> &pixels,
           int &width,
           int &height)
  {
  Imf::RgbaInputFile file (fileName);
  Imath::Box2i dw = file.dataWindow();

  width  = dw.max.x - dw.min.x + 1;
  height = dw.max.y - dw.min.y + 1;
  pixels.resizeErase (height, width);

  file.setFrameBuffer (&pixels[0][0] - dw.min.x - dw.min.y * width, 1, width);
  file.readPixels (dw.min.y, dw.max.y);
  }

SpectrumCoef_d RGBToSpectrumCoef(double i_r, double i_g, double i_b)
  {
  double r = pow(i_r/255.0,2.2);
  double g = pow(i_g/255.0,2.2);
  double b = pow(i_b/255.0,2.2);
  XYZColor_d xyz = global_sRGB_E_ColorSystem.RGB_To_XYZ(RGBColor_d(r,g,b));
  return SpectrumRoutines::XYZToSpectrumCoef(xyz);
  }

Spectrum_d RGBToSpectrum(double i_r, double i_g, double i_b)
  {
  XYZColor_d xyz = global_sRGB_D65_ColorSystem.RGB_To_XYZ(RGBColor_d(i_r,i_g,i_b));
  return SpectrumRoutines::XYZToSpectrum(xyz);
  }

inline void TestTracer::LoadMesh()
  {
  std::vector<intrusive_ptr<const Primitive> > primitives;

  //primitives.push_back(LoadWallsPrimitive("stls/walls.stl", false));
  primitives.push_back(LoadDiffusePrimitive("stls/walls.stl", false, RGBToSpectrumCoef(248,244,180)));
  primitives.push_back(LoadDiffusePrimitive("stls/floor.stl", false, RGBToSpectrumCoef(51,51,153)));
  primitives.push_back(LoadDiffusePrimitive("stls/ceiling.stl", false, RGBToSpectrumCoef(230,230,240)));
  primitives.push_back(LoadDiffusePrimitive("stls/window.stl", false, RGBToSpectrumCoef(238,194,121)));

  primitives.push_back(LoadDiffusePrimitive("stls/door.stl", false, RGBToSpectrumCoef(238,194,121)));
  primitives.push_back(LoadGlassPrimitive("stls/door_glass.stl", false, RGBToSpectrumCoef(255.0,255.0,255.0)));

  primitives.push_back(LoadDiffusePrimitive("stls/table.stl", false, RGBToSpectrumCoef(238,194,121)));
  primitives.push_back(LoadDiffusePrimitive("stls/chair1.stl", false, RGBToSpectrumCoef(127,90,48)));
  primitives.push_back(LoadDiffusePrimitive("stls/chair2.stl", false, RGBToSpectrumCoef(127,90,48)));
  primitives.push_back(LoadDiffusePrimitive("stls/chair3.stl", false, RGBToSpectrumCoef(127,90,48)));

  primitives.push_back(LoadMetalPrimitive("stls/lamp1.stl", false, RGBToSpectrumCoef(240,240,240), 0.05));
  primitives.push_back(LoadMetalPrimitive("stls/lamp2.stl", false, RGBToSpectrumCoef(240,240,240), 0.05));

  primitives.push_back(LoadDiffusePrimitive("stls/shelf1.stl", false, RGBToSpectrumCoef(240,240,240)));
  primitives.push_back(LoadDiffusePrimitive("stls/shelf2.stl", false, RGBToSpectrumCoef(240,240,240)));
  primitives.push_back(LoadDiffusePrimitive("stls/upper_shelf.stl", false, RGBToSpectrumCoef(173,216,230)));

  primitives.push_back(LoadGlassPrimitive("stls/fruit_plate.stl", true, RGBToSpectrumCoef(255,255,255)));

  primitives.push_back(LoadDiffusePrimitive("stls/oranges.stl", true, RGBToSpectrumCoef(240,165,0)));
  primitives.push_back(LoadDiffusePrimitive("stls/strawberry.stl", true, RGBToSpectrumCoef(255,3,62)));

  primitives.push_back(LoadGlassPrimitive("stls/bottles.stl", true, RGBToSpectrumCoef(255,255,255)));
  primitives.push_back(LoadDiffusePrimitive("stls/cups.stl", true, RGBToSpectrumCoef(240,240,240)));
  primitives.push_back(LoadDiffusePrimitive("stls/shoes.stl", false, RGBToSpectrumCoef(240,240,240)));

  primitives.push_back(LoadDiffusePrimitive("stls/cooker_plate.stl", false, RGBToSpectrumCoef(50,50,50)));
  primitives.push_back(LoadDiffusePrimitive("stls/saucepan_handles.stl", false, RGBToSpectrumCoef(50,50,50)));
  primitives.push_back(LoadMetalPrimitive("stls/saucepan.stl", true, RGBToSpectrumCoef(240,240,240), 0.1));
  primitives.push_back(LoadMetalPrimitive("stls/teapot.stl", true, RGBToSpectrumCoef(240,205,0), 0.1));
  primitives.push_back(LoadMetalPrimitive("stls/teapot2.stl", true, RGBToSpectrumCoef(240,205,0), 0.1));
  primitives.push_back(LoadMetalPrimitive("stls/extractor_fan.stl", true, RGBToSpectrumCoef(201,192,187), 0.1));

  primitives.push_back(LoadDiffusePrimitive("stls/frying_pan1_handle.stl", false, RGBToSpectrumCoef(50,50,50)));
  primitives.push_back(LoadDiffusePrimitive("stls/frying_pan2_handle.stl", false, RGBToSpectrumCoef(50,50,50)));
  primitives.push_back(LoadMetalPrimitive("stls/frying_pan1.stl", true, RGBToSpectrumCoef(240,40,50), 0.1));
  primitives.push_back(LoadMetalPrimitive("stls/frying_pan2.stl", true, RGBToSpectrumCoef(201,192,187), 0.1));

  LightSources lights;
/*
  intrusive_ptr<InfiniteLightSource> p_inf_light( new InfiniteLightSourceMock(2.0*RGBToSpectrum(200.0,220.0,250.0), BBox3D_d(Point3D_d(-3000,-5000,-500),Point3D_d(1500,1000,3500) ) ) );
  lights.m_infinite_light_sources.push_back(p_inf_light);

  intrusive_ptr<DeltaLightSource> p_parallel_light( new ParallelLight(Vector3D_d(-0.3,-0.5,-0.2).Normalized(), 0.8*RGBToSpectrum(M_PI*500.0, M_PI*500.0, M_PI*500.0), BBox3D_d(Point3D_d(-3000,-5000,-500),Point3D_d(1500,1000,3500) ) ) );
  lights.m_delta_light_sources.push_back(p_parallel_light);

*/
  intrusive_ptr<ImageSource<Spectrum_f> > p_image_source( new OpenEXRRgbaImageSource<Spectrum_f>("env_lights/XYZ.exr") );
  intrusive_ptr<InfiniteLightSource> p_inf_light( new ImageEnvironmentalLight(BBox3D_d(Point3D_d(-1,-1,-1)*10000,Point3D_d(1,1,1)*10000), Transform(), p_image_source) );
  lights.m_infinite_light_sources.push_back(p_inf_light);

    {
    intrusive_ptr<Texture<SpectrumCoef_d> > p_reflectance(new ConstantTexture<SpectrumCoef_d> (RGBToSpectrumCoef(212,175,55)*0.8));
    intrusive_ptr<Texture<double> > p_sigma(new ConstantTexture<double> (0.04));
    intrusive_ptr<Material> p_material(new MatteMaterial(p_reflectance, p_sigma));

    Sphere s;
    s.SetParameter("Center","-545 -1691 2130");
    s.SetParameter("Radius","30");
    s.SetParameter("Subdivisions","3");
    intrusive_ptr<TriangleMesh> p_sphere( s.BuildMesh() );
    intrusive_ptr<AreaLightSource> p_area_light( new DiffuseAreaLightSource(RGBToSpectrum(200000,200000,200000), p_sphere) );
    intrusive_ptr<Primitive> p_sphere_primitive(new Primitive(p_sphere, p_material, p_area_light));

    primitives.push_back(p_sphere_primitive);
    lights.m_area_light_sources.push_back(p_area_light);
    }

    {
    intrusive_ptr<Texture<SpectrumCoef_d> > p_reflectance(new ConstantTexture<SpectrumCoef_d> (RGBToSpectrumCoef(212,175,55)*0.8));
    intrusive_ptr<Texture<double> > p_sigma(new ConstantTexture<double> (0.04));
    intrusive_ptr<Material> p_material(new MatteMaterial(p_reflectance, p_sigma));

    Sphere s;
    s.SetParameter("Center","-1709 -600 1830");
    s.SetParameter("Radius","30");
    s.SetParameter("Subdivisions","3");
    intrusive_ptr<TriangleMesh> p_sphere( s.BuildMesh() );
    intrusive_ptr<AreaLightSource> p_area_light( new DiffuseAreaLightSource(RGBToSpectrum(300000,300000,300000), p_sphere) );
    intrusive_ptr<Primitive> p_sphere_primitive(new Primitive(p_sphere, p_material, p_area_light));

    primitives.push_back(p_sphere_primitive);
    lights.m_area_light_sources.push_back(p_area_light);
    }

    mp_scene.reset(new Scene(primitives, NULL, lights));
  }

inline void TestTracer::RenderImage()
  {
  tbb::tick_count t0, t1;
  FilmFilter *filter = new MitchellFilter(2.0,2.0);
  //intrusive_ptr<InteractiveFilm> p_film(new InteractiveFilm(GetImageWidth(), GetImageHeight(), intrusive_ptr<FilmFilter>(filter)));
  intrusive_ptr<ImageFilm> p_film(new ImageFilm(GetImageWidth(), GetImageHeight(), intrusive_ptr<FilmFilter>(filter)));
  //p_film->SetCropWindow(Point2D_i(330-3,400-28),Point2D_i(492-3,527-28));

  Point2D_i window_begin,window_end;
  p_film->GetSamplingExtent(window_begin, window_end);

/*
  // sponza
  Point3D_d camera_pos(13,0,9);
  Point3D_d look_at(0,0,2);
  Vector3D_d direction = Vector3D_d(look_at-camera_pos).Normalized();
  intrusive_ptr<Camera> p_camera( new PerspectiveCamera( MakeLookAt(camera_pos,direction,Vector3D_d(0,0,1)), p_film, 0.001*12.000, 6, 1.22) );
*/

  Vector3D_d direction = Vector3D_d(-2441-831,-590+4331,1342-1194).Normalized();
  intrusive_ptr<Camera> p_camera( new PerspectiveCamera( MakeLookAt(Point3D_d(831,-4331,1194),direction,Vector3D_d(0,0,1)), p_film, 0.001*12.000, 1300, 1.35) );

  intrusive_ptr<ImagePixelsOrder> pixel_order(new ConsecutiveImagePixelsOrder);
  //intrusive_ptr<ImagePixelsOrder> pixel_order(new RandomBlockedImagePixelsOrder);

  intrusive_ptr<Sampler> p_sampler( new LDSampler(window_begin, window_end, 16, pixel_order) );

  /*
  DirectLightingLTEIntegratorParams params;
  params.m_direct_light_samples_num=32;
  params.m_max_specular_depth=6;
  params.m_media_step_size=0.01;
  intrusive_ptr<DirectLightingLTEIntegrator> p_lte_int( new DirectLightingLTEIntegrator(mp_scene, params) );
*/  

  PhotonLTEIntegratorParams params;
  params.m_direct_light_samples_num=32;
  params.m_gather_samples_num=16;
  params.m_caustic_lookup_photons_num=100;
  params.m_max_caustic_lookup_dist=30;
  params.m_max_specular_depth=8;
  params.m_media_step_size=0.01;
  intrusive_ptr<PhotonLTEIntegrator> p_lte_int( new PhotonLTEIntegrator(mp_scene, params) );
  
  t0 = tbb::tick_count::now();
  p_lte_int->ShootPhotons(500000*0, 1000000, 2000000);
  t1 = tbb::tick_count::now();
  printf("Shooting: %lf\n", (t1-t0).seconds());

  intrusive_ptr<SamplerBasedRenderer> p_renderer( new SamplerBasedRenderer(p_lte_int, p_sampler) );
  p_renderer->SetDisplayUpdateCallback(mp_callback, 10.0);
 
  tbb::task_scheduler_init init;
  t0 = tbb::tick_count::now();
  p_renderer->Render(p_camera, true);
  t1 = tbb::tick_count::now();

  printf("Rendering: %lf\n", (t1-t0).seconds());
  p_film->ClearFilm();
  p_sampler->Reset();

/*
  std::vector<Point2D_d> samples(128*128);
  SamplingRoutines::StratifiedSampling2D(samples.begin(),128,128,true);

  for(size_t i=0;i<samples.size();++i)
    {
    Vector3D_d dir;
    double pdf;
    mp_scene->GetLightSources().m_infinite_light_sources[0]->SampleLighting(samples[i], dir, pdf);

    double theta = MathRoutines::SphericalTheta(dir);
    double phi = MathRoutines::SphericalPhi(dir);

    p_film->AddSample(Point2D_d(phi*INV_2PI*p_film->GetXResolution(), theta*INV_PI*p_film->GetYResolution()), Spectrum_d(250,250,250));
    }

  mp_callback->Update(p_film);
*/
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  }

#endif // TESTTRACER_H