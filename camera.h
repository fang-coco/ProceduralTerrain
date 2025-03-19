#ifndef CAMERA_H
#define CAMERA_H

#include <QMatrix4x4>

class Camera
{
public:
    Camera();

    QMatrix4x4 projection() const;
    void setProjection(const QMatrix4x4 &newProjection);
    QMatrix4x4 view() const;
    void setView(const QMatrix4x4 &newView);
    QSize viewportSize() const;
    void setViewportSize(const QSize &newViewportSize);
    QVector3D position() const;
    void setPosition(const QVector3D &newPosition);
    QVector3D viewVector() const;
    void setViewVector(const QVector3D &newViewVector);

public:
    void updateCameraProperties(float fov, QSize viewportSize
                                , float clipNear, float clipFar
                                , QVector3D position, QVector3D front
                                , QVector3D up);
private:
    QMatrix4x4 m_projection;
    QMatrix4x4 m_view;
    QSize m_viewportSize;
    QVector3D m_position;
    QVector3D m_viewVector;
};

#endif // CAMERA_H
