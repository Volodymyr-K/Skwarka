#ifndef RENDER_PARAMS_H
#define RENDER_PARAMS_H

#include <QObject>
#include "DirectLightParams.h"
#include "PhotonMapParams.h"

class RenderParams : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int rendererTypeIndex MEMBER m_renderer_type_index NOTIFY changed)
    Q_PROPERTY(PhotonMapParams *photonMapParams READ getPhotonMapParams CONSTANT)
    Q_PROPERTY(DirectLightParams *directLightParams READ getDirectLightParams CONSTANT)

public:
    RenderParams() {}

    int getRendererTypeIndex() const { return m_renderer_type_index; }
    void setRendererTypeIndex(int i_renderer_type_index) { m_renderer_type_index = i_renderer_type_index; emit changed(); }

    PhotonMapParams *getPhotonMapParams() { return &m_photon_map_params; }

    DirectLightParams *getDirectLightParams() { return &m_direct_light_params; }

  Q_SIGNALS:
    void changed();

  private:
    int m_renderer_type_index = 0;
    PhotonMapParams m_photon_map_params;
    DirectLightParams m_direct_light_params;
};

#endif // RENDER_PARAMS_H

