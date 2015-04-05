#ifndef PERSPECTIVE_CAMERA_PARAMS_H
#define PERSPECTIVE_CAMERA_PARAMS_H

#include <QObject>
#include <Math/Point3D.h>
#include <Math/Vector3D.h>

class PerspectiveCameraParams : public QObject
{
    Q_OBJECT

    Q_PROPERTY(double lensRadius MEMBER m_lens_radius NOTIFY changed)
    Q_PROPERTY(double focalDistance MEMBER m_focal_distance NOTIFY changed)
    Q_PROPERTY(double xViewAngle MEMBER m_x_view_angle NOTIFY changed)

public:
    PerspectiveCameraParams() {}

    double getLensRadius() const { return m_lens_radius; }
    void setLensRadius(double i_lens_radius) { m_lens_radius = i_lens_radius; emit changed(); }

    double getFocalDistance() const { return m_focal_distance; }
    void setFocalDistance(double i_focal_distance) { m_focal_distance = i_focal_distance; emit changed(); }

    double getXViewAngle() const { return m_x_view_angle; }
    void setXViewAngle(double i_x_view_angle) { m_x_view_angle = i_x_view_angle; emit changed(); }

  Q_SIGNALS:
    void changed();

  private:
    double m_lens_radius = 0.0;
    double m_focal_distance = 1.0;
    double m_x_view_angle = 1.57;
};

#endif // PERSPECTIVE_CAMERA_PARAMS_H

