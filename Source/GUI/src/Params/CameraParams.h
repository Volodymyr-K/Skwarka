#ifndef CAMERA_PARAMS_H
#define CAMERA_PARAMS_H

#include <QObject>
#include "PerspectiveCameraParams.h"

class CameraParams : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int width MEMBER m_width NOTIFY changed)
    Q_PROPERTY(int height MEMBER m_height NOTIFY changed)

    Q_PROPERTY(double positionX MEMBER m_position_x NOTIFY changed)
    Q_PROPERTY(double positionY MEMBER m_position_y NOTIFY changed)
    Q_PROPERTY(double positionZ MEMBER m_position_z NOTIFY changed)

    Q_PROPERTY(double directionX MEMBER m_direction_x NOTIFY changed)
    Q_PROPERTY(double directionY MEMBER m_direction_y NOTIFY changed)
    Q_PROPERTY(double directionZ MEMBER m_direction_z NOTIFY changed)

    Q_PROPERTY(double upX MEMBER m_up_x NOTIFY changed)
    Q_PROPERTY(double upY MEMBER m_up_y NOTIFY changed)
    Q_PROPERTY(double upZ MEMBER m_up_z NOTIFY changed)

    Q_PROPERTY(int cameraTypeIndex MEMBER m_camera_type_index NOTIFY changed)
    Q_PROPERTY(PerspectiveCameraParams *perspectiveCameraParams READ getPerspectiveCameraParams CONSTANT)

public:
    CameraParams() {}

    int getWidth() const { return m_width; }
    void setWidth(int i_width) { m_width = i_width; emit changed(); }

    int getHeight() const { return m_height; }
    void setHeight(int i_height) { m_height = i_height; emit changed(); }

    Point3D_d getPosition() const { return Point3D_d(m_position_x, m_position_y, m_position_z); }
    void setPosition(const Point3D_d &i_position) { m_position_x=i_position[0]; m_position_y=i_position[1]; m_position_z=i_position[2]; emit changed(); }

    Vector3D_d getDirection() const { return Vector3D_d(m_direction_x, m_direction_y, m_direction_z); }
    void setDirection(const Vector3D_d &i_direction) { m_direction_x=i_direction[0]; m_direction_y=i_direction[1]; m_direction_z=i_direction[2]; emit changed(); }

    Vector3D_d getUp() const { return Vector3D_d(m_up_x, m_up_y, m_up_z); }
    void setUp(const Vector3D_d &i_up) { m_up_x=i_up[0]; m_up_y=i_up[1]; m_up_z=i_up[2]; emit changed(); }

    int getCameraTypeIndex() const { return m_camera_type_index; }
    void setCameraTypeIndex(int i_camera_type_index) { m_camera_type_index = i_camera_type_index; emit changed(); }

    PerspectiveCameraParams *getPerspectiveCameraParams() { return &m_perspective_camera_params; }


  Q_SIGNALS:
    void changed();

  private:
    int m_width = 640, m_height = 480;

    double m_position_x = 0.0, m_position_y = 0.0, m_position_z = 0.0;
    double m_direction_x = 0.0, m_direction_y = 1.0, m_direction_z = 0.0;
    double m_up_x = 0.0, m_up_y = 0.0, m_up_z = 1.0;

    int m_camera_type_index = 0;
    PerspectiveCameraParams m_perspective_camera_params;
};

#endif // CAMERA_PARAMS_H

