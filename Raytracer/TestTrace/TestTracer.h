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
#include <Raytracer/Materials/Matte.h>
#include <Raytracer/Textures/ConstantTexture.h>
#include <Raytracer/Renderers/SamplerBasedRenderer.h>
#include <Raytracer/LightSources/PointLight.h>
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
#include <Raytracer/Materials/Transparent.h>
#include <Raytracer/Materials/Metal.h>
#include <Raytracer/Core/Fresnel.h>
#include <Math/CompressedDirection.h>
#include <Raytracer/Materials/Substrate.h>
#include <Raytracer/BxDFs/FresnelBlend.h>
#include <Raytracer/MicrofacetDistributions/BlinnDistribution.h>
#include "EasyBMP.h"
#include <Raytracer/LightSources/ImageEnvironmentalLight.h>
#include <Raytracer/VolumeRegions/HomogeneousVolumeRegion.h>
#include <Raytracer/VolumeRegions/GridDensityVolumeRegion.h>
#include <Raytracer/PhaseFunctions/IsotropicPhaseFunction.h>
#include <Raytracer/PhaseFunctions/MieHazyPhaseFunction.h>
#include <Raytracer/PhaseFunctions/MieMurkyPhaseFunction.h>
#include <Raytracer/PhaseFunctions/RayleighPhaseFunction.h>

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

  std::vector<std::vector<Spectrum_f> > values(Input.TellHeight(),std::vector<Spectrum_f>(Input.TellWidth()));
  for( int j=0 ; j < Input.TellHeight() ; j++)
    {
    for( int i=0 ; i < Input.TellWidth() ; i++)
      {
      values[j][i]=Spectrum_f(Input(i,j)->Red,Input(i,j)->Green,Input(i,j)->Blue)*1.0;
      values[j][i]/=255.0;
      }
    }
    
  intrusive_ptr<Mapping2D> p_mapping( new SphericalMapping2D(Point3D_d(-770,-2315,1500), Vector3D_d(0,0,1), Vector3D_d(1,0,0)) );
  intrusive_ptr< ImageTexture<Spectrum_f,Spectrum_d> > p_text(new ImageTexture<Spectrum_f,Spectrum_d>(values, p_mapping) );

  intrusive_ptr<Texture<double> > p_sigma(new ConstantTexture<double> (0.04));
  intrusive_ptr<Material> p_material(new Matte(p_text, p_sigma));

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

inline void TestTracer::LoadMesh()
  {
  std::vector<intrusive_ptr<const Primitive> > primitives;
/*
  intrusive_ptr<const Primitive> p_primitive = LoadDiffusePrimitive("sponza/sponza.stl", false, Spectrum_d(100,100,100)/255.0);
  BBox3D_d bbox = Convert<double>(p_primitive->GetTriangleMesh()->GetBounds());
  primitives.push_back(p_primitive);

  {
  intrusive_ptr<Texture<Spectrum_d> > p_refrlection(new ConstantTexture<Spectrum_d> (Spectrum_d(0.75)));
  intrusive_ptr<Texture<double> > p_roughness(new ConstantTexture<double> (0.001));
  intrusive_ptr<Material> p_material(new Metal(p_refrlection, p_roughness));

  Sphere s;
  s.SetParameter("Center","1 0 1.51");
  s.SetParameter("Radius","1.5");
  s.SetParameter("Subdivisions","7");
  intrusive_ptr<TriangleMesh> p_sphere( s.BuildMesh() );
  intrusive_ptr<Primitive> p_sphere_primitive(new Primitive(p_sphere, p_material, NULL));

  primitives.push_back(p_sphere_primitive);
  }


  LightSources lights;

  int height=0, width=0;
  Imf::Array2D<Imf::Rgba> image(height, width);
  readRgba1("env_lights/DH041LL.exr", image, width, height);

  std::vector<std::vector<Spectrum_f> > values(height, width);
  for( int j=0 ; j < height ; j++)
    for( int i=0 ; i < width ; i++)
      values[j][i] = 25*Spectrum_f(static_cast<float>(image[j][i].r), static_cast<float>(image[j][i].g), static_cast<float>(image[j][i].b));

  tbb::tick_count t0 = tbb::tick_count::now();
  intrusive_ptr<InfiniteLightSource> p_inf_light( new ImageEnvironmentalLight(bbox, Transform(), values) );
  tbb::tick_count t1 = tbb::tick_count::now();
  printf("Creating light: %lf\n", (t1-t0).seconds());

  lights.m_infinite_light_sources.push_back(p_inf_light);

  for(size_t x=0;x<4;++x) for(size_t y=0;y<2;++y)
    {
    intrusive_ptr<Texture<Spectrum_d> > p_reflectance(new ConstantTexture<Spectrum_d> (Spectrum_d(0.9)));
    intrusive_ptr<Texture<double> > p_sigma(new ConstantTexture<double> (0.04));
    intrusive_ptr<Material> p_material(new Matte(p_reflectance, p_sigma));

    std::string coord;
    if (x==0) coord = "-4.3 ";
    if (x==1) coord = "0.0 ";
    if (x==2) coord = "4.4 ";
    if (x==3) coord = "-8.8 ";

    if (y==0) coord = coord + "5.2 ";
    if (y==1) coord = coord + "-5.2 ";

    coord = coord + "4.4";

    Spectrum_d color;
    if (((x+y)%6)==0) color = Spectrum_d(1.0,0.4,0.4);
    if (((x+y)%6)==1) color = Spectrum_d(0.4,1.0,0.4);
    if (((x+y)%6)==2) color = Spectrum_d(0.4,0.4,1.0);
    if (((x+y)%6)==3) color = Spectrum_d(1.0,1.0,0.4);
    if (((x+y)%6)==4) color = Spectrum_d(0.4,1.0,1.0);
    if (((x+y)%6)==5) color = Spectrum_d(1.0,0.4,1.0);

    Sphere s;
    s.SetParameter("Center",coord.c_str());
    s.SetParameter("Radius","0.1");
    s.SetParameter("Subdivisions","3");
    intrusive_ptr<TriangleMesh> p_sphere( s.BuildMesh() );
    intrusive_ptr<AreaLightSource> p_area_light( new DiffuseAreaLightSource(3*150000*color, p_sphere) );
    intrusive_ptr<Primitive> p_sphere_primitive(new Primitive(p_sphere, p_material, p_area_light));

    primitives.push_back(p_sphere_primitive);
    lights.m_area_light_sources.push_back(p_area_light);
    }
*/

  BBox3D_d bbox;
  LightSources lights;


  BMP Input;
  Input.ReadFromFile("mendeleev.bmp");

  std::vector<std::vector<Spectrum_f> > values(Input.TellHeight(),std::vector<Spectrum_f>(Input.TellWidth()));
  for( int j=0 ; j < Input.TellHeight() ; j++)
    {
    for( int i=0 ; i < Input.TellWidth() ; i++)
      {
      values[j][i]=Spectrum_f(Input(i,j)->Red,Input(i,j)->Green,Input(i,j)->Blue)*1.0;
      values[j][i] *= 0.5/255.0;
      }
    }

  intrusive_ptr<Mapping2D> p_mapping( new UVMapping2D() );
  intrusive_ptr< ImageTexture<Spectrum_f,Spectrum_d> > p_text(new ImageTexture<Spectrum_f,Spectrum_d>(values, p_mapping) );

  /////// Add primitive 1 ///
    {
    std::vector<Point3D_f> vertices;
    std::vector<MeshTriangle> triangles;
    vertices.push_back(Point3D_f(-5,0,-5));vertices.push_back(Point3D_f(5,0,-5));
    vertices.push_back(Point3D_f(5,0,5));vertices.push_back(Point3D_f(-5,0,5));

    MeshTriangle t1(0,1,2);t1.m_uvs[0]=Point2D_f(1,1);t1.m_uvs[1]=Point2D_f(0,1);t1.m_uvs[2]=Point2D_f(0,0);
    MeshTriangle t2(2,3,0);t2.m_uvs[0]=Point2D_f(0,0);t2.m_uvs[1]=Point2D_f(1,0);t2.m_uvs[2]=Point2D_f(1,1);

    triangles.push_back(t1);triangles.push_back(t2);
    intrusive_ptr<TriangleMesh> p_ground_mesh = intrusive_ptr<TriangleMesh>( new TriangleMesh(vertices, triangles, true) );

    intrusive_ptr<Texture<double> > p_sigma(new ConstantTexture<double> (0.35));
    intrusive_ptr<Material> p_material(new Matte(p_text, p_sigma));

    intrusive_ptr<Primitive> p_primitive(new Primitive(p_ground_mesh, p_material));
    primitives.push_back(p_primitive);
    bbox.Unite(Convert<double>(p_ground_mesh->GetBounds()));
    }

    /////// Add primitive 2 ///
    {
    std::vector<Point3D_f> vertices;
    std::vector<MeshTriangle> triangles;
    vertices.push_back(Point3D_f(-5,0,4.7));vertices.push_back(Point3D_f(5,0,4.7));
    vertices.push_back(Point3D_f(5,10,4.7));vertices.push_back(Point3D_f(-5,10,4.7));
    triangles.push_back(MeshTriangle(0,1,2));triangles.push_back(MeshTriangle(2,3,0));
    intrusive_ptr<TriangleMesh> p_ground_mesh = intrusive_ptr<TriangleMesh>( new TriangleMesh(vertices, triangles, true) );

    intrusive_ptr<Texture<Spectrum_d> > p_reflectance(new ConstantTexture<Spectrum_d> (Spectrum_d(0.57,0.57,0.6)));
    intrusive_ptr<Texture<double> > p_sigma(new ConstantTexture<double> (0.35));
    intrusive_ptr<Material> p_material(new Matte(p_reflectance, p_sigma));

    intrusive_ptr<Primitive> p_primitive(new Primitive(p_ground_mesh, p_material));
    primitives.push_back(p_primitive);
    bbox.Unite(Convert<double>(p_ground_mesh->GetBounds()));
    }

  /////// Add light source  ///

    {
    Sphere s;
    s.SetParameter("Center","4 15 -2");
    s.SetParameter("Radius","0.4");
    s.SetParameter("Subdivisions","4");
    intrusive_ptr<TriangleMesh> p_sphere_mesh = s.BuildMesh();

    intrusive_ptr<Texture<Spectrum_d> > p_reflectance(new ConstantTexture<Spectrum_d> (Spectrum_d(200,200,200)/255.0*0.6));
    intrusive_ptr<Texture<double> > p_sigma(new ConstantTexture<double> (0.35));
    intrusive_ptr<Material> p_material(new Matte(p_reflectance, p_sigma));

    intrusive_ptr<const AreaLightSource> p_area_light(new DiffuseAreaLightSource(Spectrum_d(500000), p_sphere_mesh));
    intrusive_ptr<Primitive> p_primitive(new Primitive(p_sphere_mesh, p_material, p_area_light));
    //primitives.push_back(p_primitive);
    //lights.m_area_light_sources.push_back(p_area_light);
    //bbox.Unite(Convert<double>(p_sphere_mesh->GetBounds()));
    }

    // Add infinity light
    {
    int height=0, width=0;
    Imf::Array2D<Imf::Rgba> image(height, width);
    readRgba1("env_lights/DH041LL.exr", image, width, height);

    std::vector<std::vector<Spectrum_f> > values(height, width);
    for( int j=0 ; j < height ; j++)
      for( int i=0 ; i < width ; i++)
        values[j][i] = 8*Spectrum_f(static_cast<float>(image[j][i].r), static_cast<float>(image[j][i].g), static_cast<float>(image[j][i].b));

    tbb::tick_count t0 = tbb::tick_count::now();
    intrusive_ptr<InfiniteLightSource> p_inf_light( new ImageEnvironmentalLight(bbox, Transform(), values) );
    tbb::tick_count t1 = tbb::tick_count::now();
    printf("Creating light: %lf\n", (t1-t0).seconds());

    lights.m_infinite_light_sources.push_back(p_inf_light);
    }


  MieHazyPhaseFunction phase_funct;
  //intrusive_ptr<const VolumeRegion> p_volume (new HomogeneousVolumeRegion<MieHazyPhaseFunction>(
  //  BBox3D_d(Point3D_d(-70,-70,0),Point3D_d(70,70,70)), Spectrum_d(0.0), Spectrum_d(0.000), Spectrum_d(0.005), phase_funct));


  std::vector<std::vector<std::vector<double> > > densities;
  LoadDensities(densities, "densities/2.txt");

  intrusive_ptr<const VolumeRegion> p_volume (new GridDensityVolumeRegion<MieHazyPhaseFunction>(
    BBox3D_d(Point3D_d(0.01,0.01,0.51),Point3D_d(1.99,1.99,1.29)), Spectrum_d(0), Spectrum_d(3), Spectrum_d(20), phase_funct, densities) );

  mp_scene.reset(new Scene(primitives, p_volume, lights));
  }

inline void TestTracer::RenderImage()
  {
  tbb::tick_count t0, t1;
  FilmFilter *filter = new BoxFilter(0.5,0.5);
  //intrusive_ptr<InteractiveFilm> p_film(new InteractiveFilm(GetImageWidth(), GetImageHeight(), intrusive_ptr<FilmFilter>(filter)));
  intrusive_ptr<ImageFilm> p_film(new ImageFilm(GetImageWidth(), GetImageHeight(), intrusive_ptr<FilmFilter>(filter)));
  //p_film->SetCropWindow(Point2D_i(330-3,400-28),Point2D_i(492-3,527-28));

  Point2D_i window_begin,window_end;
  p_film->GetSamplingExtent(window_begin, window_end);

/*
  // sponza
  Point3D_d camera_pos(12,-1,7);
  Point3D_d look_at(-4,.2,1.5);
  Vector3D_d direction = Vector3D_d(look_at-camera_pos).Normalized();
  intrusive_ptr<Camera> p_camera( new PerspectiveCamera( MakeLookAt(camera_pos,direction,Vector3D_d(0,0,1)), p_film, 0.001*12.000, 6, 1.22) );
*/

//  Point3D_d camera_pos(7,-60,15);
//  Point3D_d look_at(7,0,15);

  Point3D_d camera_pos(3.3,4,-4);
  Point3D_d look_at(1.4,1.2,0.4);
  Vector3D_d direction = Vector3D_d(look_at-camera_pos).Normalized();
  intrusive_ptr<Camera> p_camera( new PerspectiveCamera( MakeLookAt(camera_pos,direction,Vector3D_d(0,1,0)), p_film, 0.0*12.000, 6, 0.4) );

  intrusive_ptr<ImagePixelsOrder> pixel_order(new ConsecutiveImagePixelsOrder);
  //intrusive_ptr<ImagePixelsOrder> pixel_order(new RandomBlockedImagePixelsOrder);

  intrusive_ptr<Sampler> p_sampler( new LDSampler(window_begin, window_end, 16, pixel_order) );

  DirectLightingLTEIntegratorParams params;
  params.m_direct_light_samples_num=32;
  params.m_max_specular_depth=6;
  params.m_media_step_size=0.01;
  intrusive_ptr<DirectLightingLTEIntegrator> p_lte_int( new DirectLightingLTEIntegrator(mp_scene, params) );
  
/*
  PhotonLTEIntegratorParams params;
  params.m_direct_light_samples_num=32;
  params.m_gather_samples_num=16;
  params.m_caustic_lookup_photons_num=100;
  params.m_max_caustic_lookup_dist=30;
  params.m_max_specular_depth=8;
  params.m_media_step_size=0.01;
  intrusive_ptr<PhotonLTEIntegrator> p_lte_int( new PhotonLTEIntegrator(mp_scene, params) );
  
  t0 = tbb::tick_count::now();
  p_lte_int->ShootPhotons(0, 200000, 300000);
  t1 = tbb::tick_count::now();
  printf("Shooting: %lf\n", (t1-t0).seconds());
*/
  intrusive_ptr<SamplerBasedRenderer> p_renderer( new SamplerBasedRenderer(p_lte_int, p_sampler) );
  p_renderer->SetDisplayUpdateCallback(mp_callback, 10.0);
 
  tbb::task_scheduler_init init;
  t0 = tbb::tick_count::now();
  p_renderer->Render(p_camera);
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