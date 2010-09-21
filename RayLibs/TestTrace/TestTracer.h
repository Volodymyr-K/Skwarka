#ifndef TESTTRACER_H
#define TESTTRACER_H

#include "stdafx.h"

#pragma warning(disable : 4003)
#include <Common/Common.h>

#include "tbb/pipeline.h"
#include "tbb/tick_count.h"
#include "tbb/task_scheduler_init.h"

//#include "vld.h"

#include "MeshLoader.h"
#include <Math/Geometry.h>
#include <Raytracer/Core/TriangleMesh.h>
#include <Shapes/Sphere.h>
#include <Shapes/Cylinder.h>
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
#include <Raytracer/Materials/MERLMeasuredMaterial.h>
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

SpectrumCoef_d RGBToSpectrumCoef(double i_r, double i_g, double i_b)
  {
  double r = pow(i_r,2.2);
  double g = pow(i_g,2.2);
  double b = pow(i_b,2.2);
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
  /*
    {
    std::ifstream myFile ("D:\\raytracerDB\\MERL\\brdfs\\tungsten-carbide.binary", std::ios::in | std::ios::binary);
    intrusive_ptr<MERLMeasuredData> p_merl_measured_data( new MERLMeasuredData(myFile) );
    MERLMeasured bxdf(p_merl_measured_data.get());
    

    double lums[100];
    Vector3D_d inc = Vector3D_d(1,2,3).Normalized();
    for(int i=0;i<100;++i)
      {
      double theta=M_PI_2*i/100.0;
      Vector3D_d dir=MathRoutines::SphericalDirection<double>(MathRoutines::SphericalPhi(inc)+M_PI, theta);
      lums[i] = SpectrumRoutines::Luminance(bxdf.Evaluate(inc,dir));
      }

    double pdf1, pdf2;
    Vector3D_d exitant;
    SpectrumCoef_d sp = bxdf.Sample(Vector3D_d(1,2,3).Normalized(), exitant, Point2D_d(0.4123,0.07), pdf1);
    pdf2=bxdf.PDF(Vector3D_d(1,2,3).Normalized(), exitant);

    intrusive_ptr<Material> p_material( new MERLMeasuredMaterial(p_merl_measured_data) );
    }
*/

  BBox3D_d bbox;
  std::vector<intrusive_ptr<const Primitive> > primitives;

  BMP Input;
  Input.ReadFromFile("mendeleev.bmp");

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
  intrusive_ptr<ImageSource<SpectrumCoef_f> > p_image_source( new RGB24ImageSource<SpectrumCoef_f>(values, global_sRGB_E_ColorSystem, 0.6) );

  intrusive_ptr<Mapping2D> p_mapping( new UVMapping2D(1,1) );
  intrusive_ptr< ImageTexture<SpectrumCoef_f,SpectrumCoef_d> > p_text(new ImageTexture<SpectrumCoef_f,SpectrumCoef_d>(p_image_source, p_mapping) );

  /////// Add ground primitive ///
    {
    std::vector<Point3D_f> vertices;
    std::vector<MeshTriangle> triangles;
    vertices.push_back(Point3D_f(-15,-15,0));vertices.push_back(Point3D_f(10,-15,0));
    vertices.push_back(Point3D_f(10,15,0));vertices.push_back(Point3D_f(-15,15,0));

    MeshTriangle t1(0,1,2);t1.m_uvs[0]=Point2D_f(1,1);t1.m_uvs[1]=Point2D_f(0,1);t1.m_uvs[2]=Point2D_f(0,0);
    MeshTriangle t2(2,3,0);t2.m_uvs[0]=Point2D_f(0,0);t2.m_uvs[1]=Point2D_f(1,0);t2.m_uvs[2]=Point2D_f(1,1);

    triangles.push_back(t1);triangles.push_back(t2);
    intrusive_ptr<TriangleMesh> p_ground_mesh = intrusive_ptr<TriangleMesh>( new TriangleMesh(vertices, triangles, true) );

    intrusive_ptr<Texture<double> > p_sigma(new ConstantTexture<double> (0.35));
    intrusive_ptr<Material> p_material(new MatteMaterial(p_text, p_sigma));

    intrusive_ptr<Primitive> p_primitive(new Primitive(p_ground_mesh, p_material));
    primitives.push_back(p_primitive);
    bbox.Unite(Convert<double>(p_ground_mesh->GetBounds()));
    }

  intrusive_ptr<Texture<SpectrumCoef_d> > p_reflection(new ConstantTexture<SpectrumCoef_d> (RGBToSpectrumCoef(0.75,0.75,0.75)));
  intrusive_ptr<Texture<double> > p_roughness(new ConstantTexture<double> (0.025));
  //intrusive_ptr<Material> p_material(new MetalMaterial(p_reflection, p_roughness));

  //intrusive_ptr<Texture<SpectrumCoef_d> > p_yellow_reflection(new ConstantTexture<SpectrumCoef_d> (RGBToSpectrumCoef(0.4,0.4,0.25)));
  //intrusive_ptr<Texture<SpectrumCoef_d> > p_white_reflection(new ConstantTexture<SpectrumCoef_d> (RGBToSpectrumCoef(0.4,0.4,0.4)));
  //intrusive_ptr<Material> p_material(new SubstrateMaterial(p_yellow_reflection, p_white_reflection, p_roughness));

  std::ifstream myFile ("D:\\raytracerDB\\MERL\\brdfs\\tungsten-carbide.binary", std::ios::in | std::ios::binary);

  tbb::tick_count t0 = tbb::tick_count::now();
  intrusive_ptr<MERLMeasuredData> p_merl_measured_data( new MERLMeasuredData(myFile) );
  intrusive_ptr<Material> p_material( new MERLMeasuredMaterial(p_merl_measured_data) );
  tbb::tick_count t1 = tbb::tick_count::now();
  printf("Creating MERL: %lf\n", (t1-t0).seconds());

  intrusive_ptr<Texture<SpectrumCoef_d> > p_refr_index(new ConstantTexture<SpectrumCoef_d> (SpectrumCoef_d(0.598000,0.963000,1.220000)));
  intrusive_ptr<Texture<SpectrumCoef_d> > p_abs(new ConstantTexture<SpectrumCoef_d> (SpectrumCoef_d(5.380000,6.700000,7.310000)));
  intrusive_ptr<Texture<double> > p_roughness2(new ConstantTexture<double> (0.005));
  //intrusive_ptr<Material> p_material( new MetalMaterial(p_refr_index,p_abs,p_roughness2) );
  //intrusive_ptr<Texture<double> > sig(new ConstantTexture<double> (0));
  //intrusive_ptr<Material> p_material( new MatteMaterial(p_refr_index,sig) );

  LightSources lights;
    {
    Sphere s;
    s.SetSubdivisions(5);
    s.SetTransformation(MakeTranslation(Vector3D_d(1,0,1.54))*MakeScale(2,2,2)*MakeScale(0.7));

    intrusive_ptr<TriangleMesh> p_mesh( s.BuildMesh() );
    //Transform trans = MakeTranslation(Vector3D_d(0,0.7,-5.0))*MakeTranslation(Vector3D_d(1,0,3))*MakeScale(-1,1,1)*MakeRotationZ(-M_PI_2)*MakeScale(40,40,40);
    //intrusive_ptr<TriangleMesh> p_mesh( LoadMeshFromPLY("dragon/dragon.ply", trans, true) );
    //p_mesh->SetInvertNormals(true);

    intrusive_ptr<Primitive> p_sphere_primitive(new Primitive(p_mesh, p_material, NULL));

    primitives.push_back(p_sphere_primitive);
    bbox.Unite(Convert<double>(p_mesh->GetBounds()));
    }

    {
    Cylinder c;
    c.SetSubdivisions(360);
    c.SetTransformation(MakeTranslation(Vector3D_d(1,-3.2,0.0))*MakeScale(1.5,1.5,2));
    c.SetPhiRange(M_PI*1.7,0.7*M_PI);

    intrusive_ptr<Texture<SpectrumCoef_d> > p_refr_index(new ConstantTexture<SpectrumCoef_d> (SpectrumCoef_d(0.151063,0.124375,0.125500)));
    intrusive_ptr<Texture<SpectrumCoef_d> > p_abs(new ConstantTexture<SpectrumCoef_d> (SpectrumCoef_d(2.478750,3.348125,3.766250)));
    intrusive_ptr<Texture<double> > p_roughness2(new ConstantTexture<double> (0.005));
    intrusive_ptr<Material> p_metal_material( new MetalMaterial(p_refr_index,p_abs,p_roughness2) );

    intrusive_ptr<TriangleMesh> p_mesh( c.BuildMesh() );
    intrusive_ptr<Primitive> p_cylinder_primitive(new Primitive(p_mesh, p_material, NULL));

    primitives.push_back(p_cylinder_primitive);
    bbox.Unite(Convert<double>(p_mesh->GetBounds()));
    }

    {
    intrusive_ptr<ImageSource<Spectrum_f> > p_env_light_image_source( new OpenEXRRgbaImageSource<Spectrum_f>("env_lights/DH041LL.exr", 1.0) );

    tbb::tick_count t0 = tbb::tick_count::now();
    intrusive_ptr<InfiniteLightSource> p_inf_light( new ImageEnvironmentalLight(bbox, Transform(), p_env_light_image_source) );
    tbb::tick_count t1 = tbb::tick_count::now();
    printf("Creating light: %lf\n", (t1-t0).seconds());

    lights.m_infinite_light_sources.push_back(p_inf_light);
    }

    {
    intrusive_ptr<DeltaLightSource> p_light( new PointLight(Point3D_d(1-0.2,-3.2-0.2,7.0), RGBToSpectrum(100000/M_PI,100000/M_PI,100000/M_PI)));

    lights.m_delta_light_sources.push_back(p_light);
    }

  mp_scene.reset(new Scene(primitives, NULL, lights));
  }

inline void TestTracer::RenderImage()
  {
  tbb::tick_count t0, t1;
  FilmFilter *filter = new MitchellFilter(2.0,2.0);
  //intrusive_ptr<InteractiveFilm> p_film(new InteractiveFilm(GetImageWidth(), GetImageHeight(), intrusive_ptr<FilmFilter>(filter)));
  intrusive_ptr<ImageFilm> p_film(new ImageFilm(GetImageWidth(), GetImageHeight(), intrusive_ptr<FilmFilter>(filter)));
  //p_film->SetCropWindow(Point2D_i(288-8,421-28),Point2D_i(394-8,514-28));

  Point2D_i window_begin,window_end;
  p_film->GetSamplingExtent(window_begin, window_end);

  Point3D_d camera_pos(7.5,-2.4,7);
  Point3D_d look_at(0,-1.5,2.0);
  Vector3D_d direction = Vector3D_d(look_at-camera_pos).Normalized();
  intrusive_ptr<Camera> p_camera( new PerspectiveCamera( MakeLookAt(camera_pos,direction,Vector3D_d(0,0,1)), p_film, 0.001*12.000, 6, 1.22) );

  intrusive_ptr<ImagePixelsOrder> pixel_order(new ConsecutiveImagePixelsOrder);
  //intrusive_ptr<ImagePixelsOrder> pixel_order(new RandomBlockedImagePixelsOrder);

  intrusive_ptr<Sampler> p_sampler( new LDSampler(window_begin, window_end, 8, pixel_order) );

/*
  DirectLightingLTEIntegratorParams params;
  params.m_direct_light_samples_num=8;
  params.m_max_specular_depth=6;
  params.m_media_step_size=0.01;
  intrusive_ptr<DirectLightingLTEIntegrator> p_lte_int( new DirectLightingLTEIntegrator(mp_scene, params) );
*/

  
  PhotonLTEIntegratorParams params;
  params.m_direct_light_samples_num=16;
  params.m_gather_samples_num=16;
  params.m_caustic_lookup_photons_num=100;
  params.m_max_caustic_lookup_dist=30;
  params.m_max_specular_depth=8;
  params.m_media_step_size=0.01;
  intrusive_ptr<PhotonLTEIntegrator> p_lte_int( new PhotonLTEIntegrator(mp_scene, params) );
  

  t0 = tbb::tick_count::now();
  p_lte_int->ShootPhotons(0, 4000000, 4000000);
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