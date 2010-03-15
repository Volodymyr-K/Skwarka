#include "../PhotonLTEIntegrator.h"
#include "PhotonInternalTypes.h"
#include <Common/MemoryPool.h>
#include <Math/SamplingRoutines.h>
#include <Math/ThreadSafeRandom.h>
#include <Raytracer/Core/CoreUtils.h>
#include <tbb/pipeline.h>

// 0.87 cosine value corresponds to 30 degrees angle.
const double PhotonLTEIntegrator::MAX_NORMAL_DEVIATION_COS = 0.87;

//////////////////////////////////////// PhotonLTEIntegrator /////////////////////////////////////////////
PhotonLTEIntegrator::PhotonLTEIntegrator(intrusive_ptr<const Scene> ip_scene, intrusive_ptr<VolumeIntegrator> ip_volume_integrator,
                                         intrusive_ptr<DirectLightingIntegrator> ip_direct_lighting_integrator,
                                         PhotonLTEIntegratorParams i_params):
LTEIntegrator(ip_scene, ip_volume_integrator), mp_scene(ip_scene), mp_direct_lighting_integrator(ip_direct_lighting_integrator), m_params(i_params)
  {
  ASSERT(ip_scene);
  ASSERT(ip_direct_lighting_integrator);

  if (m_params.m_max_specular_depth > 50)
    m_params.m_max_specular_depth = 50;

  m_scene_total_area = 0.0;
  const std::vector<intrusive_ptr<const Primitive> > &primitives = ip_scene->GetPrimitives();
  for(size_t i=0;i<primitives.size();++i)
    m_scene_total_area += primitives[i]->GetTriangleMesh_RawPtr()->GetArea();
  }

void PhotonLTEIntegrator::_RequestSamples(intrusive_ptr<Sampler> ip_sampler)
  {
  ASSERT(ip_sampler);
  mp_direct_lighting_integrator->RequestSamples(ip_sampler);

  if (m_params.m_gather_samples>0)
    {
    size_t gather_samples = m_params.m_gather_samples;

    m_bsdf_1D_samples_id = ip_sampler->AddSamplesSequence1D(gather_samples, &gather_samples);
    m_bsdf_2D_samples_id = ip_sampler->AddSamplesSequence2D(gather_samples, &gather_samples);

    m_direction_1D_samples_id = ip_sampler->AddSamplesSequence1D(gather_samples, &gather_samples);
    m_direction_2D_samples_id = ip_sampler->AddSamplesSequence2D(gather_samples, &gather_samples);

    ASSERT(gather_samples >= m_params.m_gather_samples);
    m_params.m_gather_samples = gather_samples;
    }
  }

void PhotonLTEIntegrator::_GetLightsPowerCDF(const LightSources &i_light_sources, std::vector<double> &o_lights_CDF)
  {
  size_t delta_lights_num = i_light_sources.m_delta_light_sources.size();
  size_t infinity_lights_num = i_light_sources.m_infinitiy_light_sources.size();
  size_t area_lights_num = i_light_sources.m_area_light_sources.size();
  size_t lights_num = delta_lights_num+infinity_lights_num + area_lights_num;

  o_lights_CDF.resize(lights_num, 0.0);
  if (lights_num == 0)
    return;

  for(size_t i=0;i<delta_lights_num;++i)
    {
    o_lights_CDF[i] = i_light_sources.m_delta_light_sources[i]->Power().Luminance();
    ASSERT(o_lights_CDF[i] >= 0.0);
    if (i>0)
      o_lights_CDF[i] += o_lights_CDF[i-1];
    }

  for(size_t i=0;i<infinity_lights_num;++i)
    {
    size_t j=delta_lights_num+i;
    o_lights_CDF[j] = i_light_sources.m_infinitiy_light_sources[i]->Power().Luminance();
    ASSERT(o_lights_CDF[j] >= 0.0);
    if (j>0)
      o_lights_CDF[j] += o_lights_CDF[j-1];
    }

  for(size_t i=0;i<area_lights_num;++i)
    {
    size_t j=delta_lights_num+infinity_lights_num+i;
    o_lights_CDF[j] = i_light_sources.m_area_light_sources[i]->Power().Luminance();
    ASSERT(o_lights_CDF[j] >= 0.0);

    if (j>0)
      o_lights_CDF[j] += o_lights_CDF[j-1];
    }

  // Normalize CDF values.
  double total_power = o_lights_CDF[lights_num-1];
  if (total_power != 0.0)
    for(size_t i=0;i<lights_num;++i)
      o_lights_CDF[i] /= total_power;
  else
    {
    // If all lights have zero power luminance we just always sample the last one.
    o_lights_CDF[lights_num-1] = 1.0;
    }
  }

std::pair<Spectrum_f, Spectrum_f>
PhotonLTEIntegrator::_LookupPhotonIrradiance(const Point3D_d &i_point, const Vector3D_d &i_normal, shared_ptr<const KDTree<Photon> > ip_photon_map,
                                             size_t i_photon_paths, size_t i_lookup_photons_num, double i_max_lookup_dist, NearestPhoton *ip_nearest_photons) const
  {
  if (ip_photon_map == NULL)
    return std::make_pair(Spectrum_f(), Spectrum_f());

  PhotonFilter filter(i_point, i_normal, MAX_NORMAL_DEVIATION_COS);
  size_t photons_found = ip_photon_map->GetNearestPoints(i_point, i_lookup_photons_num, ip_nearest_photons, filter, i_max_lookup_dist);
  if (photons_found == 0)
    return std::make_pair(Spectrum_f(), Spectrum_f());

  double max_dist_sqr = 0.0;
  Spectrum_d external_irradiance, internal_irradiance;
  for(size_t i=0;i<photons_found;++i)
    {
    Point3D_d photon_position = Convert<double>( ip_nearest_photons[i].mp_point->m_point );
    Vector3D_d photon_direction = ip_nearest_photons[i].mp_point->m_incident_direction.ToVector3D<double>();
    Spectrum_d photon_weight = Convert<double>( ip_nearest_photons[i].mp_point->m_weight );

    double tmp_dist_sqr = Vector3D_d(photon_position - i_point).LengthSqr();
    if (tmp_dist_sqr > max_dist_sqr) max_dist_sqr = tmp_dist_sqr;

    if (photon_direction * i_normal > 0.0)
      external_irradiance += photon_weight;
    else
      internal_irradiance += photon_weight;
    }

  if (photons_found<i_lookup_photons_num || max_dist_sqr==0.0)
    max_dist_sqr = i_max_lookup_dist * i_max_lookup_dist;
  else
    /*
    Since the max_dist_sqr is exactly equal to the squared distance to the farthest photon we need to multiply the area by the correcting factor.
    The easy way to understand it is the following. Think of what would happen if we decrease the radius a little bit.
    The farthest photon will drop out while the area won't change significantly. Thus the resulting radiance value would change by the value brought by the farthest photon.
    We need to increase the total area by a half of a single photon's area.
    */
    max_dist_sqr *= (photons_found) / (photons_found-0.5);

  double inv = 1.0 / (M_PI * i_photon_paths * max_dist_sqr);
  return std::make_pair(Convert<float>(external_irradiance * inv), Convert<float>(internal_irradiance * inv) );
  }

void PhotonLTEIntegrator::ShootPhotons(size_t i_caustic_photons, size_t i_direct_photons, size_t i_indirect_photons)
  {
  mp_caustic_map.reset((KDTree<Photon>*)NULL);
  mp_direct_map.reset((KDTree<Photon>*)NULL);
  mp_indirect_map.reset((KDTree<Photon>*)NULL);
  mp_irradiance_map.reset((KDTree<IrradiancePhoton>*)NULL);

  const LightSources &lights = mp_scene->GetLightSources();
  if (lights.m_delta_light_sources.size() + lights.m_area_light_sources.size() + lights.m_infinitiy_light_sources.size() == 0)
    return;

  std::vector<double> lights_CDF;
  _GetLightsPowerCDF(lights, lights_CDF);

  PhotonMaps photon_maps;
  photon_maps.m_caustic_photons.reserve(i_caustic_photons);
  photon_maps.m_direct_photons.reserve(i_direct_photons);
  photon_maps.m_indirect_photons.reserve(i_indirect_photons);

  PhotonsInputFilter input_filter(&photon_maps, i_caustic_photons, i_direct_photons, i_indirect_photons, MAX_PIPELINE_TOKENS_NUM, 4096);
  PhotonsShootingFilter shooting_filter(this, mp_scene, lights_CDF);
  PhotonsMergingFilter merging_filter(&photon_maps);

  tbb::pipeline pipeline;
  pipeline.add_filter(input_filter);
  pipeline.add_filter(shooting_filter);
  pipeline.add_filter(merging_filter);

  pipeline.run(MAX_PIPELINE_TOKENS_NUM);
  pipeline.clear();

  if (photon_maps.m_caustic_photons.empty()==false)
    mp_caustic_map.reset( new KDTree<Photon>(photon_maps.m_caustic_photons) );

  if (photon_maps.m_direct_photons.empty()==false)
    mp_direct_map.reset( new KDTree<Photon>(photon_maps.m_direct_photons) );

  if (photon_maps.m_indirect_photons.empty()==false)
    mp_indirect_map.reset( new KDTree<Photon>(photon_maps.m_indirect_photons) );

  m_caustic_paths = photon_maps.m_caustic_paths;
  m_direct_paths = photon_maps.m_direct_paths;
  m_indirect_paths = photon_maps.m_indirect_paths;

  // Select positions for irradiance photons.
  // We pick each 10th photon from each photon map.
  std::vector<IrradiancePhoton> irradiance_photons;
  for(size_t i=0;i<photon_maps.m_caustic_photons.size();i+=10)
    irradiance_photons.push_back( IrradiancePhoton(photon_maps.m_caustic_photons[i].m_point, Spectrum_f(), Spectrum_f(), photon_maps.m_caustic_photons[i].m_normal) );

  for(size_t i=0;i<photon_maps.m_direct_photons.size();i+=10)
    irradiance_photons.push_back( IrradiancePhoton(photon_maps.m_direct_photons[i].m_point, Spectrum_f(), Spectrum_f(), photon_maps.m_direct_photons[i].m_normal) );

  for(size_t i=0;i<photon_maps.m_indirect_photons.size();i+=10)
    irradiance_photons.push_back( IrradiancePhoton(photon_maps.m_indirect_photons[i].m_point, Spectrum_f(), Spectrum_f(), photon_maps.m_indirect_photons[i].m_normal) );

  // Estimate lookup radius so that the corresponding area will hold required number of photons (in average).
  // We multiply m_scene_total_area by 2 because each surface has two sides.
  double direct_photon_area = photon_maps.m_direct_photons.empty() ? 0.0 : 2.0*m_scene_total_area / photon_maps.m_direct_photons.size();
  double max_direct_lookup_dist = sqrt(direct_photon_area*LOOKUP_PHOTONS_NUM_FOR_IRRADIANCE*INV_PI);

  double indirect_photon_area = photon_maps.m_indirect_photons.empty() ? 0.0 : 2.0*m_scene_total_area / photon_maps.m_indirect_photons.size();
  double max_indirect_lookup_dist = sqrt(indirect_photon_area*LOOKUP_PHOTONS_NUM_FOR_IRRADIANCE*INV_PI);

  // Not sure what's a better way to estimate caustic lookup radius.
  double max_caustic_lookup_dist = std::max(max_direct_lookup_dist, max_indirect_lookup_dist);

  NearestPhoton *p_nearest_photons = new NearestPhoton[LOOKUP_PHOTONS_NUM_FOR_IRRADIANCE];
  for(size_t i=0;i<irradiance_photons.size();++i)
    {
    Point3D_d point = Convert<double>(irradiance_photons[i].m_point);
    Vector3D_d normal = irradiance_photons[i].m_normal.ToVector3D<double>();

    std::pair<Spectrum_f, Spectrum_f> caustic_irradiance  =
      _LookupPhotonIrradiance(point, normal, mp_caustic_map, m_caustic_paths, LOOKUP_PHOTONS_NUM_FOR_IRRADIANCE, max_caustic_lookup_dist, p_nearest_photons);

    std::pair<Spectrum_f, Spectrum_f> direct_irradiance   =
      _LookupPhotonIrradiance(point, normal, mp_direct_map, m_direct_paths, LOOKUP_PHOTONS_NUM_FOR_IRRADIANCE, max_direct_lookup_dist, p_nearest_photons);

    std::pair<Spectrum_f, Spectrum_f> indirect_irradiance =
      _LookupPhotonIrradiance(point, normal, mp_indirect_map, m_indirect_paths, LOOKUP_PHOTONS_NUM_FOR_IRRADIANCE, max_indirect_lookup_dist, p_nearest_photons);

    irradiance_photons[i].m_external_irradiance = caustic_irradiance.first+direct_irradiance.first+indirect_irradiance.first;
    irradiance_photons[i].m_internal_irradiance = caustic_irradiance.second+direct_irradiance.second+indirect_irradiance.second;
    }

  // Free memory before building a new KDTree.
  delete[] p_nearest_photons;
  photon_maps.m_caustic_photons.swap(std::vector<Photon>());
  photon_maps.m_direct_photons.swap(std::vector<Photon>());
  photon_maps.m_indirect_photons.swap(std::vector<Photon>());

  if (irradiance_photons.empty()==false)
    mp_irradiance_map.reset( new KDTree<IrradiancePhoton>(irradiance_photons) );
  }

Spectrum_d PhotonLTEIntegrator::_SurfaceRadiance(const RayDifferential &i_ray, const Intersection &i_intersection, const Sample *ip_sample, MemoryPool &i_pool) const
  {
  ASSERT(i_ray.m_base_ray.m_direction.IsNormalized());
  Spectrum_d radiance;

  const BSDF *p_bsdf = i_intersection.mp_primitive->GetBSDF(i_intersection.m_dg, i_intersection.m_triangle_index, i_pool);
  Vector3D_d incident = i_ray.m_base_ray.m_direction*(-1.0);

  // Add emitting lighting from the surface (if the surface has light source properties).
  const AreaLightSource *p_light_source = i_intersection.mp_primitive->GetAreaLightSource_RawPtr();
  if (p_light_source)
    radiance = p_light_source->Radiance(i_intersection.m_dg, i_intersection.m_triangle_index, incident);

  bool has_non_specular = p_bsdf->GetComponentsNum( BxDFType(BSDF_ALL & ~BSDF_SPECULAR) ) > 0;
  if (has_non_specular)
    {
    // Compute direct lighting.
    radiance += mp_direct_lighting_integrator->ComputeDirectLighting(i_intersection, incident, p_bsdf, ip_sample, i_pool);

    radiance += _LookupCausticRadiance(p_bsdf, i_intersection.m_dg, incident, i_pool);

    // Compute indirect lighting by shooting final gather rays.
    if (mp_irradiance_map)
      radiance += _FinalGather(i_intersection, incident, p_bsdf, ip_sample, i_pool);    
      
/*
    const size_t samples_num_sqrt = 5;
    Point2D_d bsdf_scattering_samples[samples_num_sqrt*samples_num_sqrt];
    SamplesSequence2D bsdf_scattering_sequence(bsdf_scattering_samples, bsdf_scattering_samples + samples_num_sqrt*samples_num_sqrt);
    SamplingRoutines::StratifiedSampling2D(bsdf_scattering_sequence.m_begin, samples_num_sqrt, samples_num_sqrt, false);

    IrradiancePhotonFilter filter(i_intersection.m_dg.m_point, i_intersection.m_dg.m_shading_normal, MAX_NORMAL_DEVIATION_COS);
    const IrradiancePhoton *p_irradiance_photon = mp_irradiance_map->GetNearestPoint(i_intersection.m_dg.m_point, filter);
    if (p_irradiance_photon)
      {
      Spectrum_d tmp;
      if (incident*i_intersection.m_dg.m_shading_normal > 0.0)
        {
        tmp += p_bsdf->TotalScattering(incident, bsdf_scattering_sequence, BxDFType(BSDF_ALL_REFLECTION))  *Convert<double>(p_irradiance_photon->m_external_irradiance);
        tmp += p_bsdf->TotalScattering(incident, bsdf_scattering_sequence, BxDFType(BSDF_ALL_TRANSMISSION))*Convert<double>(p_irradiance_photon->m_internal_irradiance);
        }
      else
        {
        tmp += p_bsdf->TotalScattering(incident, bsdf_scattering_sequence, BxDFType(BSDF_ALL_REFLECTION))  *Convert<double>(p_irradiance_photon->m_internal_irradiance);
        tmp += p_bsdf->TotalScattering(incident, bsdf_scattering_sequence, BxDFType(BSDF_ALL_TRANSMISSION))*Convert<double>(p_irradiance_photon->m_external_irradiance);
        }

      tmp *= INV_PI;
      radiance += tmp;
      }
*/
    }

  // Trace rays for specular reflection and refraction.
  if (i_ray.m_specular_depth <= m_params.m_max_specular_depth)
    {
    radiance += _SpecularReflect(i_ray, i_intersection, p_bsdf, ip_sample, i_pool);
    radiance += _SpecularTransmit(i_ray, i_intersection, p_bsdf, ip_sample, i_pool);
    }

  return radiance;
  }

Spectrum_d PhotonLTEIntegrator::_FinalGather(const Intersection &i_intersection, const Vector3D_d &i_incident, const BSDF *ip_bsdf, const Sample *ip_sample, MemoryPool &i_pool) const
  {
  ASSERT(ip_bsdf);
  ASSERT(mp_irradiance_map);
  ASSERT(i_incident.IsNormalized());

  const double cos_gather_angle = 0.9848; // 10 degrees
  const double cone_pdf = SamplingRoutines::UniformConePDF(cos_gather_angle);

  BxDFType non_specular = BxDFType(BSDF_REFLECTION | BSDF_TRANSMISSION | BSDF_DIFFUSE | BSDF_GLOSSY);
  size_t gather_samples = m_params.m_gather_samples;
  if (gather_samples == 0)
    return Spectrum_d();

  // Maybe it is worth to put this block out of the _FinalGather() method to improve the performance.
  const size_t samples_num_sqrt = 5;
  Point2D_d bsdf_scattering_samples[samples_num_sqrt*samples_num_sqrt];
  SamplesSequence2D bsdf_scattering_sequence(bsdf_scattering_samples, bsdf_scattering_samples + samples_num_sqrt*samples_num_sqrt);
  SamplingRoutines::StratifiedSampling2D(bsdf_scattering_sequence.m_begin, samples_num_sqrt, samples_num_sqrt, false);

  SamplesSequence1D bsdf_1D_samples, direction_1D_samples;
  SamplesSequence2D bsdf_2D_samples, direction_2D_samples;
  if (ip_sample)
    {
    bsdf_1D_samples = ip_sample->GetSamplesSequence1D(m_bsdf_1D_samples_id);
    bsdf_2D_samples = ip_sample->GetSamplesSequence2D(m_bsdf_2D_samples_id);

    direction_1D_samples = ip_sample->GetSamplesSequence1D(m_direction_1D_samples_id);
    direction_2D_samples = ip_sample->GetSamplesSequence2D(m_direction_2D_samples_id);
    }
  else
    {
    size_t gather_samples_sqrt = (size_t) ceil(sqrt((double)gather_samples));
    gather_samples = gather_samples_sqrt*gather_samples_sqrt;

    // BSDF samples.
    double *bsdf_samples_1D = (double *)i_pool.Alloc( gather_samples * sizeof(double) );
    Point2D_d *bsdf_samples_2D = (Point2D_d *)i_pool.Alloc( gather_samples * sizeof(Point2D_d) );

    bsdf_1D_samples = SamplesSequence1D(bsdf_samples_1D, bsdf_samples_1D+gather_samples);
    bsdf_2D_samples = SamplesSequence2D(bsdf_samples_2D, bsdf_samples_2D+gather_samples);

    SamplingRoutines::StratifiedSampling1D(bsdf_1D_samples.m_begin, gather_samples, true);
    SamplingRoutines::StratifiedSampling2D(bsdf_2D_samples.m_begin, gather_samples_sqrt, gather_samples_sqrt, true);

    // Direction samples.
    double *direction_samples_1D = (double *)i_pool.Alloc( gather_samples * sizeof(double) );
    Point2D_d *direction_samples_2D = (Point2D_d *)i_pool.Alloc( gather_samples * sizeof(Point2D_d) );

    direction_1D_samples = SamplesSequence1D(direction_samples_1D, direction_samples_1D+gather_samples);
    direction_2D_samples = SamplesSequence2D(direction_samples_2D, direction_samples_2D+gather_samples);

    SamplingRoutines::StratifiedSampling1D(direction_1D_samples.m_begin, gather_samples, true);
    SamplingRoutines::StratifiedSampling2D(direction_2D_samples.m_begin, gather_samples_sqrt, gather_samples_sqrt, true);
    }

  // Allocate array for the nearest photons.
  size_t photons_found = 0;
  NearestPhoton *p_nearest_photons = (NearestPhoton*)i_pool.Alloc(32 * sizeof(NearestPhoton));

  // Search for nearest indirect photons.
  if (mp_indirect_map && mp_indirect_map->GetNumberOfPoints()>0)
    {
    double indirect_photon_area = 2.0*m_scene_total_area / mp_indirect_map->GetNumberOfPoints();
    double max_lookup_dist = sqrt(indirect_photon_area*32*INV_PI);
    PhotonFilter filter(i_intersection.m_dg.m_point, i_intersection.m_dg.m_shading_normal, MAX_NORMAL_DEVIATION_COS);
    photons_found = mp_indirect_map->GetNearestPoints(i_intersection.m_dg.m_point, 32, p_nearest_photons, filter, max_lookup_dist);
    }
  double inv_photons_found = (photons_found>0) ? 1.0/photons_found : 0.0;

  // Copy photon directions to a local array.
  Vector3D_d *photon_directions = (Vector3D_d*)i_pool.Alloc(photons_found * sizeof(Vector3D_d));
  for (size_t i=0;i<photons_found;++i)
    photon_directions[i] = p_nearest_photons[i].mp_point->m_incident_direction.ToVector3D<double>();

  size_t gather_rays = 0;
  Vector3D_d *p_gather_directions = (Vector3D_d*)i_pool.Alloc( 2 * gather_samples * sizeof(Vector3D_d) );
  Spectrum_d *p_gather_weights = (Spectrum_d*)i_pool.Alloc( 2 * gather_samples * sizeof(Spectrum_d) );

  // Sample BSDF.
  SamplesSequence1D::Iterator iterator_1D = bsdf_1D_samples.m_begin;
  SamplesSequence2D::Iterator iterator_2D = bsdf_2D_samples.m_begin;
  for (size_t i=0;i<gather_samples;++i)
    {
    double component_sample = *iterator_1D;
    Point2D_d bxdf_sample = *iterator_2D;

    double bsdf_pdf = 0.0;
    BxDFType sampled_type;
    Vector3D_d exitant;
    Spectrum_d reflectance = ip_bsdf->Sample(i_incident, exitant, bxdf_sample, component_sample, bsdf_pdf, sampled_type, BxDFType(BSDF_ALL & ~BSDF_SPECULAR));
    if (bsdf_pdf>0.0 && reflectance.IsBlack()==false)
      {
      // Compute PDF for photon-sampling for the sampled direction.
      // We construct a cone around each photon and sample directions in the cone uniformly.
      double photon_pdf = 0.0;
      for (size_t j=0;j<photons_found;++j)
        if (photon_directions[j]*exitant > cos_gather_angle)
          photon_pdf += cone_pdf;
      photon_pdf *= inv_photons_found;

      double weight = SamplingRoutines::PowerHeuristic(gather_samples, bsdf_pdf, gather_samples, photon_pdf);

      p_gather_directions[gather_rays] = exitant;
      p_gather_weights[gather_rays] = reflectance * (weight * fabs(exitant*i_intersection.m_dg.m_shading_normal) / bsdf_pdf);
      ++gather_rays;
      }

    ++iterator_1D;
    ++iterator_2D;
    }

  // Sample nearby photons.
  iterator_1D = direction_1D_samples.m_begin;
  iterator_2D = direction_2D_samples.m_begin;
  for (size_t i=0;i<gather_samples && photons_found>0;++i)
    {
    size_t sampled_index = (size_t)( (*iterator_1D) * photons_found);

    // Sample gather ray direction.
    Vector3D_d e2, e3;
    MathRoutines::CoordinateSystem(photon_directions[sampled_index], e2, e3);
    Vector3D_d direction_local = SamplingRoutines::UniformConeSampling(*iterator_2D, cos_gather_angle);
    Vector3D_d exitant = direction_local[0]*e2 + direction_local[1]*e3 + direction_local[2]*photon_directions[sampled_index];

    Spectrum_d reflectance = ip_bsdf->Evaluate(i_incident, exitant);
    if (reflectance.IsBlack()==false)
      {
      double bsdf_pdf = ip_bsdf->PDF(i_incident, exitant);

      // Compute PDF for photon-sampling of direction exitant.
      // We construct a cone around each photon and sample directions in the cone uniformly.
      double photon_pdf = 0.0;
      for (size_t j=0;j<photons_found;++j)
        if (j==sampled_index || photon_directions[j]*exitant > cos_gather_angle)
          photon_pdf += cone_pdf;
      photon_pdf *= inv_photons_found;

      double weight = SamplingRoutines::PowerHeuristic(gather_samples, photon_pdf, gather_samples, bsdf_pdf);

      p_gather_directions[gather_rays] = exitant;
      p_gather_weights[gather_rays] = reflectance * (weight * fabs(exitant*i_intersection.m_dg.m_shading_normal) / photon_pdf);
      ++gather_rays;
      }

    ++iterator_1D;
    ++iterator_2D;
    }

  // Trace final gather rays and compute the radiance.
  Spectrum_d radiance;
  for(size_t i=0;i<gather_rays;++i)
    {
    Ray bounce_ray(i_intersection.m_dg.m_point, p_gather_directions[i]);
    bounce_ray.m_min_t = CoreUtils::GetNextMinT(i_intersection, p_gather_directions[i]);
  
    Intersection gather_isect;
    if (mp_scene->Intersect(RayDifferential(bounce_ray), gather_isect, &bounce_ray.m_max_t))
      {
      // Compute exitant radiance at the final gather intersection.
      Vector3D_d gather_direction = bounce_ray.m_direction*(-1.0);
      const BSDF *p_gather_BSDF = gather_isect.mp_primitive->GetBSDF(gather_isect.m_dg, gather_isect.m_triangle_index, i_pool);

      IrradiancePhotonFilter filter(gather_isect.m_dg.m_point, gather_isect.m_dg.m_shading_normal, MAX_NORMAL_DEVIATION_COS);
      const IrradiancePhoton *p_irradiance_photon = mp_irradiance_map->GetNearestPoint(gather_isect.m_dg.m_point, filter);
      if (p_irradiance_photon == NULL)
        continue;

      Spectrum_d tmp;
      if (gather_direction*gather_isect.m_dg.m_shading_normal > 0.0)
        {
        tmp += p_gather_BSDF->TotalScattering(gather_direction, bsdf_scattering_sequence, BxDFType(BSDF_ALL_REFLECTION))  *Convert<double>(p_irradiance_photon->m_external_irradiance);
        tmp += p_gather_BSDF->TotalScattering(gather_direction, bsdf_scattering_sequence, BxDFType(BSDF_ALL_TRANSMISSION))*Convert<double>(p_irradiance_photon->m_internal_irradiance);
        }
      else
        {
        tmp += p_gather_BSDF->TotalScattering(gather_direction, bsdf_scattering_sequence, BxDFType(BSDF_ALL_REFLECTION))  *Convert<double>(p_irradiance_photon->m_internal_irradiance);
        tmp += p_gather_BSDF->TotalScattering(gather_direction, bsdf_scattering_sequence, BxDFType(BSDF_ALL_TRANSMISSION))*Convert<double>(p_irradiance_photon->m_external_irradiance);
        }

      tmp *= INV_PI;
      radiance += tmp * _VolumeTransmittance(bounce_ray, NULL) * p_gather_weights[i];
      }
    }

  return radiance / gather_samples;
  }

Spectrum_d PhotonLTEIntegrator::_LookupCausticRadiance(const BSDF *ip_bsdf, const DifferentialGeometry &i_dg, const Vector3D_d &i_direction, MemoryPool &i_pool) const
  {
  ASSERT(ip_bsdf);
  ASSERT(i_direction.IsNormalized());

  if (mp_caustic_map == NULL)
    return Spectrum_d();

  BxDFType non_specular = BxDFType(BSDF_REFLECTION | BSDF_TRANSMISSION | BSDF_DIFFUSE | BSDF_GLOSSY);
  if (ip_bsdf->GetComponentsNum(non_specular) == 0)
    return Spectrum_d();

  // Allocate array for the nearest photons.
  NearestPhoton *p_nearest_photons = (NearestPhoton*)i_pool.Alloc(m_params.m_caustic_lookup_photons_num * sizeof(NearestPhoton));

  PhotonFilter filter(i_dg.m_point, i_dg.m_shading_normal, MAX_NORMAL_DEVIATION_COS);
  size_t photons_found = mp_caustic_map->GetNearestPoints(i_dg.m_point, m_params.m_caustic_lookup_photons_num, p_nearest_photons, filter, m_params.m_max_caustic_lookup_dist);
  if (photons_found == 0)
    return Spectrum_d();

  double max_dist_sqr = 0.0;
  Spectrum_d radiance;
  for(size_t i=0;i<photons_found;++i)
    {
    Point3D_d photon_position = Convert<double>( p_nearest_photons[i].mp_point->m_point );
    Vector3D_d photon_direction = p_nearest_photons[i].mp_point->m_incident_direction.ToVector3D<double>();
    Spectrum_d photon_weight = Convert<double>( p_nearest_photons[i].mp_point->m_weight );

    double tmp_dist_sqr = Vector3D_d(photon_position - i_dg.m_point).LengthSqr();
    if (tmp_dist_sqr > max_dist_sqr) max_dist_sqr = tmp_dist_sqr;

    double kernel = _PhotonKernel( Vector3D_d(photon_position-i_dg.m_point).LengthSqr(), max_dist_sqr);
    radiance += ip_bsdf->Evaluate(photon_direction, i_direction) * photon_weight * kernel;
    }

  if (photons_found<<m_params.m_caustic_lookup_photons_num || max_dist_sqr==0.0)
    max_dist_sqr = m_params.m_max_caustic_lookup_dist * m_params.m_max_caustic_lookup_dist;
  else
    /*
    Since the max_dist_sqr is exactly equal to the squared distance to the farthest photon we need to multiply the area by the correcting factor.
    The easy way to understand it is the following. Think of what would happen if we decrease the radius a little bit.
    The farthest photon will drop out while the area won't change significantly. Thus the resulting radiance value would change by the value brought by the farthest photon.
    We need to increase the total area by a half of a single photon's area.
    */
    max_dist_sqr *= (photons_found) / (photons_found-0.5);

  return radiance / (m_caustic_paths * max_dist_sqr);
  }

double PhotonLTEIntegrator::_PhotonKernel(double i_dist_sqr, double i_max_dist_sqr) const
  {
  ASSERT(i_dist_sqr>=0.0 && i_max_dist_sqr>0.0 && i_dist_sqr<=i_max_dist_sqr);

  // Simpsonís kernel function is used.
  double tmp = (1.0 - i_dist_sqr / i_max_dist_sqr);
  return 3.0 * INV_PI * tmp * tmp;
  }
