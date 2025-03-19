#include "manager.h"

Manager::Manager(QObject *parent)
    : QObject{parent}
    , m_camera(new Camera)
{
    connect(this, &Manager::updateCamera
          , this, [this](float fov, QSize viewportSize
                       , float clipNear, float clipFar
                       , QVector3D position, QVector3D front
                       , QVector3D up) {
        // qDebug() << "camera update";
        m_camera->updateCameraProperties(fov, viewportSize, clipNear, clipFar, position, front, up);
    });
}
