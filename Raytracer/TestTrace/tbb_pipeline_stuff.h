#ifndef TBB_PIPELINE_STUFF_H
#define TBB_PIPELINE_STUFF_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/Camera.h>
#include <Raytracer/Core/TriangleTree.h>
#include <Raytracer/Core/Sample.h>
#include <Raytracer/Core/Sampler.h>
#include <Raytracer/Core/Primitive.h>
#include <Raytracer/Core/Intersection.h>
#include <Common/MemoryPool.h>
#include <Raytracer/Core/BSDF.h>
#include "tbb/pipeline.h"
#include <Raytracer/MicrofacetDistributions/BlinnDistribution.h>
#include <Raytracer/BxDFs/Microfacet.h>

class TestTracer;

#define CHUNK_SAMPLES 16

struct SampleChunk
  {
  intrusive_ptr<Sample> mp_sample[CHUNK_SAMPLES];
  Spectrum_f m_spectrum[CHUNK_SAMPLES];
  MemoryPool pool;

  size_t chunk_index,samples;
  };

static const size_t n_chunks = 16;
SampleChunk m_chunks[n_chunks];
bool chunks_free[n_chunks];

class MyInputFilter: public tbb::filter
  {
  public:
    MyInputFilter( Sampler *ip_sampler );

  private:
    Sampler *mp_sampler;
    /*override*/ void* operator()(void*);

    size_t m_next_chunk;
  };

MyInputFilter::MyInputFilter( Sampler *ip_sampler ): filter(serial_in_order),mp_sampler(ip_sampler), m_next_chunk(0)
  {
  for(int i=0;i<n_chunks;++i)
    chunks_free[i]=true;
  }

void* MyInputFilter::operator()(void*)
  {
  while(chunks_free[m_next_chunk]==false)
    m_next_chunk = (m_next_chunk+1) % n_chunks;

  chunks_free[m_next_chunk]=false;

  SampleChunk &chunk = m_chunks[m_next_chunk];
  for(size_t i=0;i<CHUNK_SAMPLES;++i)
    {
    if (chunk.mp_sample[i].get()==NULL)
      chunk.mp_sample[i]=mp_sampler->CreateSample();
    }

  chunk.chunk_index=m_next_chunk;

  m_next_chunk = (m_next_chunk+1) % n_chunks;

  chunk.samples=0;
  for(int i=0;i<CHUNK_SAMPLES;++i)
    {
    bool f = mp_sampler->GetNextSample(chunk.mp_sample[i]);
    if (f)
      ++chunk.samples;
    else
      break;
    }

  if (chunk.samples==0)
    return NULL;
  else
    return &chunk;
  }

class MyTransformFilter: public tbb::filter
  {
  public:
   MyTransformFilter(Camera *ip_camera, TriangleTree *ip_tree);
    /*override*/void* operator()( void* item );
  private:
    Camera *mp_camera;
    TriangleTree *mp_tree;
  };

MyTransformFilter::MyTransformFilter(Camera *ip_camera, TriangleTree *ip_tree):  tbb::filter(parallel),  mp_camera(ip_camera), mp_tree(ip_tree)
  {
  }

/*override*/void* MyTransformFilter::operator()( void* item )
  {
  SampleChunk &chunk = *static_cast<SampleChunk*>(item);

  for(size_t i=0;i<chunk.samples;++i)
    {
    Ray ray,rx,ry;
    mp_camera->GenerateRay(chunk.mp_sample[i]->GetImagePoint(), chunk.mp_sample[i]->GetLensUV(), ray);
    mp_camera->GenerateRay(chunk.mp_sample[i]->GetImagePoint()+Point2D_d(1,0), chunk.mp_sample[i]->GetLensUV(), rx);
    mp_camera->GenerateRay(chunk.mp_sample[i]->GetImagePoint()+Point2D_d(0,1), chunk.mp_sample[i]->GetLensUV(), ry);

    RayDifferential rd;
    rd.m_has_differentials=true;
    rd.m_base_ray=ray;
    rd.m_origin_dx=rx.m_origin;
    rd.m_origin_dy=ry.m_origin;
    rd.m_direction_dx=rx.m_direction;
    rd.m_direction_dy=ry.m_direction;

    Intersection isect;
    if (mp_tree->Intersect(rd, isect)==false)
      chunk.m_spectrum[i]=Spectrum_f(0.f, 0.f, 255.f);
    else
      { 
      BSDF *p_bsdf=isect.mp_primitive->GetBSDF(isect.m_dg, isect.m_triangle_index, chunk.pool);

      //BSDF *p_bsdf = new ( chunk.pool.Alloc(sizeof(BSDF)) ) BSDF(isect.m_dg);

      Spectrum_d gold_color = Spectrum_d(212,175,55)/255.0;
      Spectrum_d refractive_index, absorption;
      ApproximateFresnelParameters(gold_color, refractive_index, absorption);

      FresnelConductor fresnel(refractive_index, absorption);
      BlinnDistribution blinn(100.0);
      typedef Microfacet<FresnelConductor,BlinnDistribution> Metal;
      BxDF *p_bxdf = new ( chunk.pool.Alloc(sizeof(Metal)) ) Metal(Spectrum_d(0.2), fresnel, blinn);

      p_bsdf->AddBxDF(p_bxdf);

      Vector3D_d view_direction = Vector3D_d(0,-0.5,-1).Normalized();
      Vector3D_d light_direction = Vector3D_d(0,-0.2,-1).Normalized();
      Spectrum_d color = p_bsdf->Evaluate(light_direction,view_direction)* (p_bsdf->GetShadingNormal()*light_direction)*(-3.0);
      //color.Clamp(0.0,DBL_INF);
      chunk.m_spectrum[i]=Spectrum_f((float)color[0]*255.f, (float)color[1]*255.f, (float)color[2]*255.f);
      }

    chunk.pool.FreeAll();
    }

  return &chunk;
  }

//! Filter that writes each buffer to a file.
class MyOutputFilter: public tbb::filter
  {
  public:
    MyOutputFilter( intrusive_ptr<Film> ip_film, HWND &ig_hWnd, HDC &ig_memDC, TestTracer *ip_tracer );
    /*override*/void* operator()( void* item );
  private:
    intrusive_ptr<Film> mp_film;
    const HWND &g_hWnd;
    const HDC &g_memDC;
    TestTracer *p_tracer;
  };

MyOutputFilter::MyOutputFilter( intrusive_ptr<Film> ip_film, HWND &ig_hWnd, HDC &ig_memDC, TestTracer *ip_tracer ) : tbb::filter(serial_out_of_order), mp_film(ip_film),
g_hWnd(ig_hWnd), g_memDC(ig_memDC), p_tracer(ip_tracer)
  {
  }

extern int pixel_counter;

void* MyOutputFilter::operator()( void* item )
  {
  SampleChunk &chunk = *static_cast<SampleChunk*>(item);
  for(size_t i=0;i<chunk.samples;++i)
    {
  mp_film->AddSample(chunk.mp_sample[i]->GetImagePoint(), chunk.m_spectrum[i]);
    }
  chunks_free[chunk.chunk_index]=true;
    
/*
  ++pixel_counter;
  if ((pixel_counter%2000)!=0) return NULL;

  for(int y=0;y<p_tracer->GetImageHeight();++y)
    for(int x=0;x<p_tracer->GetImageWidth();++x)
      {
      Spectrum_f sp;
      mp_film->GetPixel(Point2D_i(x,y),sp);

      unsigned int pixel_index = (y*p_tracer->GetImageWidth()+x)*4;
      Byte* pixel = p_tracer->m_image;
      pixel[pixel_index+0] = Byte(std::min(sp[0],255.f));
      pixel[pixel_index+1] = Byte(std::min(sp[1],255.f));
      pixel[pixel_index+2] = Byte(std::min(sp[2],255.f));
      }

    BitBlt(GetDC(g_hWnd), 0, 0, p_tracer->GetImageWidth(), p_tracer->GetImageHeight(), g_memDC, 0, 0, SRCCOPY);
*/
  return NULL;
  }

#endif // TBB_PIPELINE_STUFF_H