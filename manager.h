#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QQuickItem>
#include "camera.h"

class Manager : public QObject
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit Manager(QObject *parent = nullptr);

    Q_INVOKABLE Camera* getCamera() const { return m_camera.get(); }

signals:
    void updateCamera(float fov, QSize viewportSize
                    , float clipNear, float clipFar
                    , QVector3D position, QVector3D front
                    , QVector3D up);
private:
    QScopedPointer<Camera> m_camera;
};

#endif // MANAGER_H
