#ifndef PHOTON_MAP_PARAMS_H
#define PHOTON_MAP_PARAMS_H

#include <QObject>

class PhotonMapParams : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int photonPaths MEMBER m_photon_paths NOTIFY changed)
    Q_PROPERTY(int maxCausticPhotons MEMBER m_max_caustic_photons NOTIFY changed)
    Q_PROPERTY(int maxDirectPhotons MEMBER m_max_direct_photons NOTIFY changed)
    Q_PROPERTY(int maxIndirectPhotons MEMBER m_max_indirect_photons NOTIFY changed)
    Q_PROPERTY(int samplesPerPixel MEMBER m_samples_per_pixel NOTIFY changed)
    Q_PROPERTY(int directLightSamplesNum MEMBER m_direct_light_samples NOTIFY changed)
    Q_PROPERTY(int finalGatherSamples MEMBER m_final_gather_samples NOTIFY changed)
    Q_PROPERTY(int causticLookupPhotonsNum MEMBER m_caustic_lookup_photons_num NOTIFY changed)
    Q_PROPERTY(double causticLookupDist MEMBER m_caustic_lookup_dist NOTIFY changed)
    Q_PROPERTY(int maxSpecularDepth MEMBER m_max_specular_depth NOTIFY changed)
    Q_PROPERTY(double mediaStepSize MEMBER m_media_step_size NOTIFY changed)

public:
    PhotonMapParams() {}

    int getPhotonPaths() const { return m_photon_paths; }
    void setPhotonPaths(int i_photon_paths) { m_photon_paths = i_photon_paths; emit changed(); }

    int getMaxCausticPhotons() const { return m_max_caustic_photons; }
    void setMaxCausticPhotons(int i_max_caustic_photons) { m_max_caustic_photons = i_max_caustic_photons; emit changed(); }

    int getMaxDirectPhotons() const { return m_max_direct_photons; }
    void setMaxDirectPhotons(int i_max_direct_photons) { m_max_direct_photons = i_max_direct_photons; emit changed(); }

    int getMaxIndirectPhotons() const { return m_max_indirect_photons; }
    void setMaxIndirectPhotons(int i_max_indirect_photons) { m_max_indirect_photons = i_max_indirect_photons; emit changed(); }

    int getSamplesPerPixel() const { return m_samples_per_pixel; }
    void setSamplesPerPixel(int i_samples_per_pixel) { m_samples_per_pixel = i_samples_per_pixel; emit changed(); }

    int getDirectLightSamples() const { return m_direct_light_samples; }
    void setDirectLightSamples(int i_direct_light_samples) { m_direct_light_samples = i_direct_light_samples; emit changed(); }

    int getFinalGatherSamples() const { return m_final_gather_samples; }
    void setFinalGatherSamples(int i_final_gather_samples) { m_final_gather_samples = i_final_gather_samples; emit changed(); }

    int getCausticLookupPhotonsNum() const { return m_caustic_lookup_photons_num; }
    void setCausticLookupPhotonsNum(int i_caustic_lookup_photons_num) { m_caustic_lookup_photons_num = i_caustic_lookup_photons_num; emit changed(); }

    double getCausticLookupDist() const { return m_caustic_lookup_dist; }
    void setCausticLookupDist(double i_caustic_lookup_dist) { m_caustic_lookup_dist = i_caustic_lookup_dist; emit changed(); }

    int getSpecularDepth() const { return m_max_specular_depth; }
    void setSpecularDepth(int i_max_specular_depth) { m_max_specular_depth = i_max_specular_depth; emit changed(); }

    double getMediaStepSize() const { return m_media_step_size; }
    void setMediaStepSize(double i_media_step_size) { m_media_step_size = i_media_step_size; emit changed(); }

  Q_SIGNALS:
    void changed();

  private:
    int m_photon_paths = 10;
    int m_max_caustic_photons = 0, m_max_direct_photons = 0, m_max_indirect_photons = 0;
    int m_samples_per_pixel = 4;
    int m_direct_light_samples = 8;
    int m_final_gather_samples = 8;
    int m_caustic_lookup_photons_num = 100;
    double m_caustic_lookup_dist = 0.05;
    int m_max_specular_depth = 10;
    double m_media_step_size = 0.05;
};

#endif // PHOTON_MAP_PARAMS_H

