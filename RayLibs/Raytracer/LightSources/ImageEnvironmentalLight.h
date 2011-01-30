#ifndef IMAGE_ENVIRONMENTAL_LIGHT_H
#define IMAGE_ENVIRONMENTAL_LIGHT_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/LightSources.h>
#include <Raytracer/Core/MIPMap.h>
#include <Raytracer/Core/Spectrum.h>
#include <vector>

/**
* Environment light source that uses 2D array of spectrum values (i.e. image) to determine radiance coming from different directions.
* The light allows for sampling light directions with PDF based on the radiance luminance and cosine factor therefore providing
* best possible sampling strategy (with cost of some memory requirements).
* The implementation constructs kD-tree on the image map. Each leaf corresponds to a non-empty rectangle on the image map. All leaves of the tree cover the entire image.
* For each possible normal (discretized with CompressedDirection) the probability (to be sampled) for each tree node is precomputed.
* Having the probabilities the sampling comes down to constructing a path from the root of the tree to one of the leaves each time using children's probabilities to decide which path to go.
* Inside each leaf the texels are sampled based on their radiance luminance.
*/
class ImageEnvironmentalLight: public InfiniteLightSource
  {
  public:
    /**
    * Constructs ImageEnvironmentalLight with the specified image.
    * @param i_world_bounds World bounds.
    * @param i_light_to_world Transform object that defines transformation from the light space to the world space.
    * @param i_image 2D array of the image values. All inner vectors should have the same size. Should have at least one row and at least one column.
    * @param i_scale Scale factor for the image values.
    */
    ImageEnvironmentalLight(const BBox3D_d &i_world_bounds, const Transform &i_light_to_world,
      const std::vector<std::vector<Spectrum_f> > &i_image, SpectrumCoef_d i_scale = SpectrumCoef_d(1.0));

    /**
    * Constructs ImageEnvironmentalLight from the specified image source.
    * @param i_world_bounds World bounds.
    * @param i_light_to_world Transform object that defines transformation from the light space to the world space.
    * @param ip_image_source ImageSource implementation that defines image for the ImageEnvironmentalLight. The image defined by the ImageSource should not be empty.
    * @param i_scale Scale factor for the image values.
    */
    ImageEnvironmentalLight(const BBox3D_d &i_world_bounds, const Transform &i_light_to_world,
      intrusive_ptr<const ImageSource<Spectrum_f> > ip_image_source, SpectrumCoef_d i_scale = SpectrumCoef_d(1.0));

    /**
    * Returns the light source radiance for the specified ray.
    * The ray differentials are used to determine proper filter width.
    * @param i_ray The ray pointing to the light source.
    * @return Light radiance.
    */
    Spectrum_d Radiance(const RayDifferential &i_ray) const;

    /**
    * Returns the total power of the light source, i.e. the light flux.
    * The power is estimated by computing the flux for the scene bounding box.
    */
    Spectrum_d Power() const;

    /**
    * Samples direct lighting.
    * The direction is sampled with the PDF roughly proportional to the light radiance luminance.
    * @param i_sample 2D sample. Should be in [0;1)^2 range.
    * @param[out] o_lighting_direction Direction to the light source from the specified point. Should be normalized.
    * @param[out] o_pdf PDF value for the sampled direction. The returned value should be greater or equal than zero.
    * @return Radiance value.
    */
    Spectrum_d SampleLighting(const Point2D_d &i_sample, Vector3D_d &o_lighting_direction, double &o_pdf) const;

    /**
    * Returns PDF value for the specified point and lighting direction.
    * The PDF is roughly proportional to the light radiance luminance.
    * @param i_lighting_direction Direction to the light source from the specified point. The vector is normalized.
    * @return PDF value for the sampled direction. The returned value should be greater or equal than zero.
    */
    double LightingPDF(const Vector3D_d &i_lighting_direction) const;

    /**
    * Samples direct lighting for the specified surface normal.
    * The direction is sampled with the PDF roughly proportional to the light radiance luminance multiplied by the cosine factor.
    * @param i_normal Surface normal vector at the specified point. Should be normalized.
    * @param i_sample 2D sample. Should be in [0;1)^2 range.
    * @param[out] o_lighting_direction Direction to the light source from the specified point. Should be normalized.
    * @param[out] o_pdf PDF value for the sampled direction. The returned value should be greater or equal than zero.
    * @return Radiance value.
    */
    Spectrum_d SampleLighting(const Vector3D_d &i_normal, const Point2D_d &i_sample, Vector3D_d &o_lighting_direction, double &o_pdf) const;

    /**
    * Returns PDF value for the specified point, surface normal vector and lighting direction.
    * The PDF roughly proportional to the light radiance luminance multiplied by the cosine factor.
    * @param i_normal Surface normal vector at the specified point. Should be normalized.
    * @param i_lighting_direction Direction to the light source from the specified point. The vector is normalized.
    * @return PDF value for the sampled direction. The returned value should be greater or equal than zero.
    */
    double LightingPDF(const Vector3D_d &i_normal, const Vector3D_d &i_lighting_direction) const;

    /**
    * Samples outgoing light ray.
    * @param i_position_sample 2D sample used to sample photon ray origin. Should be in [0;1)^2 range.
    * @param i_direction_sample 2D sample used to sample photon ray direction. Should be in [0;1)^2 range.
    * @param[out] o_photon_ray Sampled ray. The direction component of the ray should be normalized. The ray origin should be outside of the world bounds.
    * @param[out] o_pdf PDF value for the sampled light ray. The returned value should be greater or equal than zero.
    * @return Irradiance value.
    */
    Spectrum_d SamplePhoton(const Point2D_d &i_position_sample, const Point2D_d &i_direction_sample, Ray &o_photon_ray, double &o_pdf) const;

    /**
    * Returns irradiance value at a surface point with the specified normal assuming there's no objects in the scene blocking the light.
    * Only light comping from the positive hemisphere (with respect to the specified normal) is considered.
    * The method returns an approximation rather than the exact value.
    * @param i_normal Surface normal. Should be normalized.
    * @return Irradiance value.
    */
    Spectrum_d Irradiance(const Vector3D_d &i_normal) const;

    /**
    * Returns fluence value at a point (radiance integrated over the sphere) assuming there's no objects in the scene blocking the light.
    * The method does not take the surface normal so it accounts for the light coming from the entire sphere.
    * The method returns an approximation rather than the exact value.
    * @return Fluence value.
    */
    Spectrum_d Fluence() const;

  private:

    // Node of the kD-tree used to subdivide the image map.
    struct Node
      {
      // Total radiance of the image map region corresponding to the node.
      Spectrum_d m_total_radiance;

      // Begin and end points of the image map rectangle.
      Point2D_i m_image_begin, m_image_end;

      // Index of the right child
      size_t m_right_child;

      union
        {
        // Used only for internal nodes.
        size_t m_split_coordinate;

        // Index of the start element in ImageEnvironmentalLight::m_CDF_rows for this node (only for leaves).
        size_t m_CDF_begin;
        };

      // Defines split axis (0 for X, 1 for Y).
      unsigned char m_split_axis;


      // True for leaf and false for internal node.
      bool m_leaf;
      };

    // Maximum depth of the tree. Higher values increase PDF accuracy but require more memory.
    // IMPORTANT! It is very important not to make this value too high because the memory requirements grow exponentially. Value of 8 is probably the best trade-off.
    static const size_t MAX_TREE_DEPTH = 8;

  private:
    void _Initialize();

    void _Build(size_t i_node_index, size_t i_depth, const Point2D_i &i_begin, const Point2D_i &i_end, size_t &io_next_free_node_index);

    void _PrecomputeData();

    Spectrum_d _PrecomputeIrradiance(size_t i_node_index, const Vector3D_d &i_normal, const Point2D_d i_normal_angles, float *op_nodes_PDF) const;

    Spectrum_d _LightingSample(Point2D_d i_sample, const float *ip_nodes_pdf, Vector3D_d &o_lighting_direction, double &o_pdf) const;

    double _LightingPDF(const Vector3D_d &i_lighting_direction, const float *ip_nodes_pdf) const;

  private:
    ImageEnvironmentalLight() {} // Empty default constructor for the boost serialization framework.

    // Needed for the boost serialization framework.  
    friend class boost::serialization::access;

    /**
    * Saves ImageEnvironmentalLight to the specified Archive. This method is called by the serialize() method.
    */
    template<class Archive>
    void save(Archive &i_ar, const unsigned int i_version) const;

    /**
    * Loads ImageEnvironmentalLight from the specified Archive. This method is called by the serialize() method.
    */
    template<class Archive>
    void load(Archive &i_ar, const unsigned int i_version);

    /**
    * Serializes ImageEnvironmentalLight to/from the specified Archive. This method is used by the boost serialization framework.
    */
    template<class Archive>
    void serialize(Archive &i_ar, const unsigned int i_version);

  private:
    BBox3D_d m_world_bounds;

    Transform m_light_to_world, m_world_to_light;

    SpectrumCoef_d m_scale;

    // MIP map used to filter radiance values.
    intrusive_ptr<const MIPMap<Spectrum_f> > mp_image_map;
    size_t m_width, m_height;
    double m_theta_coef, m_phi_coef;

    // Nodes of the kD-tree.
    Node m_nodes[(1<<MAX_TREE_DEPTH)*2 - 1];
    size_t m_nodes_num;

    // Contains directions (in light space) corresponding to each node's directions.
    // Contains 4 elements for each node for each of the rectangle's corners.
    std::vector<Vector3D_d> m_nodes_directions;

    // Irradiance values for all normals (2^16 total).
    std::vector<Spectrum_d> m_irradiances;

    // PDFs for all normals (2^16 total) for sampling with normal provided.
    // The elements of the vector are grouped into 2^16 blocks each with as many elements as there are nodes in the tree.
    std::vector<float> m_nodes_hemispherical_PDF;

    // PDF for sampling with no normal provided. Contains as many elements as there are nodes in the tree.
    std::vector<float> m_nodes_spherical_PDF;

    // CDFs for selecting row to be sampled.
    // The vector contains concatenated CDFs for all leaves and each leaf stores the index of the first element of it's CDf sequence.
    std::vector<double> m_CDF_rows;

    // CDFs for selection column to be sampled.
    // Has the same size that the image map has and each row contains concatenated CDFs for all leaves intersecting the row.
    std::vector<std::vector<double> > m_CDF_cols;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class Archive>
void ImageEnvironmentalLight::save(Archive &i_ar, const unsigned int i_version) const
  {
  i_ar & m_world_bounds;
  i_ar & m_light_to_world;
  i_ar & m_world_to_light;
  i_ar & mp_image_map;
  i_ar & m_height;
  i_ar & m_width;
  i_ar & m_scale;
  }

template<class Archive>
void ImageEnvironmentalLight::load(Archive &i_ar, const unsigned int i_version)
  {
  i_ar & m_world_bounds;
  i_ar & m_light_to_world;
  i_ar & m_world_to_light;
  i_ar & mp_image_map;
  i_ar & m_height;
  i_ar & m_width;
  i_ar & m_scale;

  // Clear vectors with old data.
  m_nodes_directions.clear();
  m_irradiances.clear();
  m_nodes_hemispherical_PDF.clear();
  m_nodes_spherical_PDF.clear();
  m_CDF_rows.clear();
  m_CDF_cols.clear();

  _Initialize();
  }

template<class Archive>
void ImageEnvironmentalLight::serialize(Archive &i_ar, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<InfiniteLightSource>(*this);
  boost::serialization::split_member(i_ar, *this, i_version);
  }

// Register the derived class in the boost serialization framework.
BOOST_CLASS_EXPORT(ImageEnvironmentalLight)

#endif // IMAGE_ENVIRONMENTAL_LIGHT_H