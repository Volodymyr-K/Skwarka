#ifndef DIRECT_LIGHT_PARAMS_H
#define DIRECT_LIGHT_PARAMS_H

#include <QObject>

class DirectLightParams : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int samplesPerPixel MEMBER m_samples_per_pixel NOTIFY changed)
    Q_PROPERTY(int directLightSamplesNum MEMBER m_direct_light_samples NOTIFY changed)
    Q_PROPERTY(int maxSpecularDepth MEMBER m_max_specular_depth NOTIFY changed)
    Q_PROPERTY(double mediaStepSize MEMBER m_media_step_size NOTIFY changed)

public:
    DirectLightParams() {}

    int getSamplesPerPixel() const { return m_samples_per_pixel; }
    void setSamplesPerPixel(int i_samples_per_pixel) { m_samples_per_pixel = i_samples_per_pixel; emit changed(); }

    int getDirectLightSamples() const { return m_direct_light_samples; }
    void setDirectLightSamples(int i_direct_light_samples) { m_direct_light_samples = i_direct_light_samples; emit changed(); }

    int getSpecularDepth() const { return m_max_specular_depth; }
    void setSpecularDepth(int i_max_specular_depth) { m_max_specular_depth = i_max_specular_depth; emit changed(); }

    double getMediaStepSize() const { return m_media_step_size; }
    void setMediaStepSize(double i_media_step_size) { m_media_step_size = i_media_step_size; emit changed(); }

  Q_SIGNALS:
    void changed();

  private:
    int m_samples_per_pixel = 4;
    int m_direct_light_samples = 8;
    int m_max_specular_depth = 10;
    double m_media_step_size = 0.05;
};

#endif // DIRECT_LIGHT_PARAMS_H

