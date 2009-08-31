#ifndef TBB_PIPELINE_STUFF_H
#define TBB_PIPELINE_STUFF_H

#include <Common\Common.h>
#include <Math\Geometry.h>
#include <Raytracer\Core\Camera.h>
#include <Raytracer\Core\TriangleTree.h>
#include <Raytracer\Core\Sample.h>
#include <Raytracer\Core\Sampler.h>
#include "tbb/pipeline.h"

struct SampleChunk
  {
  shared_ptr<Sample> mp_sample;
  Spectrum_f m_spectrum;
  float m_alfa;
  };

class MyInputFilter: public tbb::filter
  {
  public:
    static const size_t n_chunks = 8;
    MyInputFilter( Sampler *ip_sampler );

  private:
    Sampler *mp_sampler;
    size_t m_next_chunk;

    SampleChunk m_chunks[n_chunks];
    /*override*/ void* operator()(void*);
  };

MyInputFilter::MyInputFilter( Sampler *ip_sampler ): filter(serial_in_order),mp_sampler(ip_sampler), m_next_chunk(0)
  {
  }

void* MyInputFilter::operator()(void*)
  {
  SampleChunk &chunk = m_chunks[m_next_chunk];
  if (chunk.mp_sample.get()==NULL)
    chunk.mp_sample=mp_sampler->CreateSample();

  m_next_chunk = (m_next_chunk+1) % n_chunks;

  bool f = mp_sampler->GetNextSample(chunk.mp_sample);
  if (f==false)
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

  Ray ray;
  mp_camera->GenerateRay(chunk.mp_sample->GetImagePoint(), chunk.mp_sample->GetLensUV(), ray);

  DifferentialGeometry dg;
  IntersectResult result = mp_tree->Intersect(ray);

  bool hit;
  if (result.m_intersection_found==false)
    hit=false;
  else
    {
    RayDifferential rd;
    rd.m_base_ray=ray;
    result.mp_mesh->ComputeDifferentialGeometry(result.m_triangle_index,rd,dg);
    hit=true;
    }

  if (hit==false)
    {
    chunk.m_spectrum=Spectrum_f(255.f, 0.f, 0.f);
    chunk.m_alfa=0.f;
    }
  else
    { 
    Vector3D_d light_corner(0.0,0.25,0.17);
    light_corner+=Vector3D_d(0,-0.5,-1).Normalized()*0.0+Vector3D_d(0,1,0)*0.25;

    Vector3D_d light_v1=0.2*Vector3D_d(0,-0.5,-1).Normalized();
    Vector3D_d light_v2=0.2*(Vector3D_d(0,-1,0)^light_v1).Normalized();
    Vector3D_d light_normal = (light_v1^light_v2).Normalized();

    double lighting=0;

    SamplesSequence2D seq = chunk.mp_sample->GetSamplesSequence2D(0);
    for(SamplesSequence2D::IteratorType it = seq.m_begin; it!=seq.m_end; ++it)
      {
      Point2D_d sample2D = *it;
      Vector3D_d light_point = light_corner + light_v1*sample2D[0] + light_v2*sample2D[1];

      Ray shadow_ray = Ray(dg.m_point, (light_point-Vector3D_d(dg.m_point)).Normalized(), 0.0001);
      IntersectResult shadow_result = mp_tree->Intersect(shadow_ray);
      if (shadow_result.m_intersection_found==false)
        {
        lighting+=0.003*(20.0/std::distance(seq.m_begin,seq.m_end))/(light_point-Vector3D_d(dg.m_point)).LengthSqr();
        }
      }

    //Vector3D_d direction = Vector3D_d(0,-0.5,-1).Normalized();
    //float color = fabsf(-(dg.m_shading_normal*direction));
    //chunk.m_spectrum=Spectrum_f(color*255.f, color*255.f, color*255.f);
    chunk.m_spectrum=Spectrum_f(lighting*255.f, lighting*255.f, lighting*255.f);
    chunk.m_alfa=1.f;
    }

  return &chunk;
  }

//! Filter that writes each buffer to a file.
class MyOutputFilter: public tbb::filter
  {
  public:
    MyOutputFilter( shared_ptr<Film> ip_film );
    /*override*/void* operator()( void* item );
  private:
    shared_ptr<Film> mp_film;
  };

MyOutputFilter::MyOutputFilter( shared_ptr<Film> ip_film ) : tbb::filter(serial_out_of_order), mp_film(ip_film)
  {
  }

void* MyOutputFilter::operator()( void* item )
  {
  SampleChunk &chunk = *static_cast<SampleChunk*>(item);
  mp_film->AddSample(chunk.mp_sample->GetImagePoint(), chunk.m_spectrum, chunk.m_alfa);
  return NULL;
  }

#endif // TBB_PIPELINE_STUFF_H