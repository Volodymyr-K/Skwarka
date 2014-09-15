#ifndef MERL_MEASURED_H
#define MERL_MEASURED_H

#include <Common/Common.h>
#include <Raytracer/Core/BxDF.h>
#include <vector>
#include <istream>
#include <Math/HalfFloat.h>

/**
* The class stores measured reflectivity data and provides methods to evaluate and sample the data and compute the sampling PDF.
* The reflectivity data is stored in regular half-angle format described in the article here http://www.merl.com/papers/docs/TR2003-083.pdf
* The reflectivity data is stored as Spectrum_h type to reduce the memory requirements.
*
* The class also precomputes and stores PDF values that are used for efficient sampling of the BRDF function.
* The PDF data is stored as array of 2D "layers", each layer corresponds to a range of theta angles of the incident vector.
* The whole domain of theta angles of the incident vector, which is [0;PI/2], is subdivided in a number of equal regions and a 2D layer is associated with each such region.
* Each 2D layer (called Segmentation2D in the code) defines PDF for sampling the exitant vector for the corresponding theta angle of the incident vector.
* This PDF is defined by a 2D array, each row corresponds to a region of theta angles of the exitant vector and each column corresponds to a region of phi angles of the exitant vector.
* Values in each row define the CDF function that is used for sampling the phi angle. Additional 1D vector defines CDF function that is used for sampling the theta angle.
* The regions of theta and phi angles which correspond to rows and columns of the table define a grid which subdivides the sampling domain.
* This grid is not regular, it is computed in a specific way to decrease the variance of the sampling.
* Basically, the regions where the BRDF function takes high values are covered more densely by the grid and vice versa.
* @sa MERLMeasured
*/
class MERLMeasuredData: public ReferenceCounted
  {
  public:
    /**
    * Creates MERLMeasuredData with the data that read from the specified input stream.
    * The data in the stream must follow the format defined for the MERL BRDF data.
    */
    MERLMeasuredData(std::istream &i_stream);

    /**
    * Returns BRDF value for the specified incident and exitant directions.
    */
    SpectrumCoef_d GetBRDF(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const;

    /**
    * Samples BRDF function for the specified incident direction.
    * The incident direction, as well as the sampled exitant direction must be in the positive hemisphere.
    */
    SpectrumCoef_d Sample(const Vector3D_d &i_incident, Vector3D_d &o_exitant, const Point2D_d &i_sample, double &o_pdf) const;
    
    /**
    * Returns PDF value for the specified incident and exitant direction.
    * The incident and exitant directions must be in the positive hemisphere.
    */
    double PDF(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const;

  private:
    // Inner class used to represent the PDF data for a specific incident theta angle.
    class Segmentation2D;

  private:
    MERLMeasuredData() {}; // Empty default constructor for the boost serialization framework.

    void _InitializeSegmentations();

  private:
    // Needed for the boost serialization framework.  
    friend class boost::serialization::access;

    /**
    * Serializes MERLMeasuredData to/from the specified Archive. This method is used by the boost serialization framework.
    */
    template<class Archive>
    void serialize(Archive &i_ar, const unsigned int i_version);

  private:
    // The three values below define the resolution of the MERL BRDF data and are defined by the format. Don't change them.
    const static size_t BRDF_SAMPLING_RES_THETA_H = 90;
    const static size_t BRDF_SAMPLING_RES_THETA_D = 90;
    const static size_t BRDF_SAMPLING_RES_PHI_D = 180;

    // The three values below define resolution of subdivision of the sampling domain.
    // Change them wisely, bigger values require more memory and processing time while smaller values increase the sampling variance.
    const static size_t SEGMENTATION_INCIDENT_THETA_RES = 90;
    const static size_t SEGMENTATION_EXITANT_THETA_RES = 24;
    const static size_t SEGMENTATION_EXITANT_PHI_RES = 45;

    // Defines the multiplier for the SEGMENTATION_EXITANT_THETA_RES and SEGMENTATION_EXITANT_PHI_RES for the "potential" positions of the grid lines.
    // Change it wisely, bigger values require more processing time and do not necessarily yield a better result.
    const static size_t SEGMENTATION_MULTIPLIER = 8;

    std::vector<SpectrumCoef_h> m_brdf_data;

    std::vector<Segmentation2D> m_segmentations;
  };

/**
* Reflective isotropic BxDF implementation based on measured MERL BRDF reflectance data.
* MERL BRDF data is represented as a set of measurements for a dense set of incident and exitant vectors.
* The instances of the class are initialized with a reference to the MERLMeasuredData which holds the actual data.
* The same instance of MERLMeasuredData can be reused for initializing multiple MERLMeasured instances to avid the cost of preprocessing the data each time.
* See http://www.merl.com/areas/brdf/ and http://www.merl.com/papers/docs/TR2003-083.pdf for the details.
* @sa MERLMeasuredData
*/
class MERLMeasured: public BxDF
  {
  public:
    /**
    * Creates MERLMeasured instance with the specified reflectivity data.
    * @param ip_merl_measured_data MERLMeasuredData pointer. Should not be NULL.
    * @warning The class does not own the passed MERLMeasuredData instance nor will it try to delete it. The calling code needs to take care of that.
    */
    MERLMeasured(const MERLMeasuredData *ip_merl_measured_data);

    /**
    * Returns BxDF value for the specified incident and exitant directions.
    * The method redirects the call to the MERLMeasuredData instance.
    */
    virtual SpectrumCoef_d Evaluate(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const;

    /**
    * Samples BxDF value for the specified incident direction.
    * The sampled exitant direction may be in any hemisphere, regardless of the incident direction.
    * The method redirects the call to the MERLMeasuredData instance.
    * @param i_incident Incident direction. Should be normalized.
    * @param[out] o_exitant Exitant direction. The returned value should be normalized.
    * @param i_sample 2D sample. Should be in [0;1]x[0;1] range.
    * @param[out] o_pdf PDF value for the sampled exitant direction. The returned value should be greater or equal than zero.
    * @return Sampled BxDF value.
    */
    virtual SpectrumCoef_d Sample(const Vector3D_d &i_incident, Vector3D_d &o_exitant, const Point2D_d &i_sample, double &o_pdf) const;

    /**
    * Returns PDF value for the specified incident and exitant direction.
    * The method redirects the call to the MERLMeasuredData instance.
    * @param i_incident Incident direction. Should be normalized.
    * @param i_exitant Exitant direction. Should be normalized.
    * @return PDF value. Greater or equal than zero.
    */
    virtual double PDF(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const;

  private:
    const MERLMeasuredData *mp_merl_measured_data;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////// MERLMeasuredData::Segmentation2D //////////////////////////////////////

/**
* Defines 2D PDF function for sampling the exitant vector.
* The PDF function is defined by a 2D array, each row of which represents CDF function for the corresponding theta angle.
* Rows and columns of the table correspond to the regions of theta and phi angles of the domain respectively and together define a grid on the domain.
* The grid is not regular, the position of the grid lines are positioned in a specific way to reduce the sampling variance.
*/
class MERLMeasuredData::Segmentation2D
  {
  public:
    /**
    * Initializes Segmentation2D with the specified 2D array of luminance values.
    * The input array will be reduced to the necessary size by merging some adjacent rows and columns (see _Reduce() method).
    */
    Segmentation2D(const std::vector<std::vector<float> > &i_values);

    /**
    * Samples 2D point on the [0;1]x[0;1] domain with the PDF defined by the class instance.
    * Instead of returning exact sampled position the method only computes the cell in the 2D array the sample value corresponds to
    * and computes the residual sample value remapped with respect to the cell's box.
    * This is convenient if the PDF function inside each cell is not uniform so the application can sample it properly.
    * @param i_sample 2D sample. Should be in [0;1]x[0;1] range.
    * @param[out] o_residual_sample Residual remapped sample value. Should be in [0;1]x[0;1] range.
    * @param[out] o_box_min Min point of the box of the sampled cell. Should be in [0;1]x[0;1] range.
    * @param[out] o_box_max Max point of the box of the sampled cell. Should be in [0;1]x[0;1] range.
    * @param[out] o_pdf Sampling probability of the sampled cell.
    */
    void Sample(const Point2D_d &i_sample, Point2D_d &o_residual_sample, Point2D_d &o_box_min, Point2D_d &o_box_max, double &o_pdf) const;

    /**
    * Returns PDF value for the specified point.
    * @param i_point The point in the [0;1]x[0;1] domain to get PDF value for.
    * @param[out] o_box_min Min point of the box of the corresponding cell. Should be in [0;1]x[0;1] range.
    * @param[out] o_box_max Max point of the box of the corresponding cell. Should be in [0;1]x[0;1] range.
    * @param[out] o_pdf Sampling probability of the corresponding cell.
    */
    void PDF(const Point2D_d &i_point, Point2D_d &o_box_min, Point2D_d &o_box_max, double &o_pdf) const;

  private:
    /**
    * Helper method that reduces the input vector to the specified size by merging adjacent elements.
    * The method uses Haffman-like algorithm to merge the smallest elements and leave the largest elements unmerged.
    * The method returns vector of the specified size with values defining the positions in the original (unmerged) vector.
    * These positions correspond to the first elements in each merged region.
    */
    std::vector<size_t> _Reduce(std::vector<float> i_values, size_t i_size) const;

  private:
    Segmentation2D() {}; // Empty default constructor for the boost serialization framework.

    // Needed for the boost serialization framework.  
    friend class boost::serialization::access;

    /**
    * Serializes Segmentation2D to/from the specified Archive. This method is used by the boost serialization framework.
    */
    template<class Archive>
    void serialize(Archive &i_ar, const unsigned int i_version);

  private:
    // Define grid of the domain subdivision.
    std::vector<float> m_grid_X, m_grid_Y;

    // CDF function for sampling the rows (theta values).
    std::vector<float> m_CDF_rows;

    // Array of CDF functions for sampling the columns (phi values).
    float m_CDF_cols[SEGMENTATION_EXITANT_THETA_RES][SEGMENTATION_EXITANT_PHI_RES];
  };

template<class Archive>
void MERLMeasuredData::Segmentation2D::serialize(Archive &i_ar, const unsigned int i_version)
  {
  i_ar & m_grid_X;
  i_ar & m_grid_Y;
  i_ar & m_CDF_rows;
  i_ar & m_CDF_cols;
  }

////////////////////////////////////////// MERLMeasuredData ///////////////////////////////////////////////

template<class Archive>
void MERLMeasuredData::serialize(Archive &i_ar, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<ReferenceCounted>(*this);
  i_ar & m_brdf_data;
  i_ar & m_segmentations;
  }

#endif // MERL_MEASURED_H
