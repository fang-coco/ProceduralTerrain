#include "camera.h"

Camera::Camera() {}

void Camera::updateCameraProperties(float fov, QSize viewportSize, float clipNear, float clipFar, QVector3D position, QVector3D front, QVector3D up)
{

    m_viewportSize = viewportSize;
    m_position = position;
    m_viewVector = front;

    m_projection.setToIdentity();
    float aspect = (float)m_viewportSize.width() / m_viewportSize.height();
    m_projection.perspective(fov, aspect, clipNear, clipFar);

    // // Calculate the right vector: normalize the cross product of the up vector and front vector
    // QVector3D right = QVector3D::crossProduct({0, 1, 0}, front).normalized();

    // // Recalculate the up vector: normalize the cross product of front and right vectors
    // QVector3D newUp = QVector3D::crossProduct(front, right).normalized();

    m_view.setToIdentity();
    m_view.lookAt(m_position, m_position + front, up);
}

QMatrix4x4 Camera::projection() const
{
    return m_projection;
}

void Camera::setProjection(const QMatrix4x4 &newProjection)
{
    m_projection = newProjection;
}

QMatrix4x4 Camera::view() const
{
    return m_view;
}

void Camera::setView(const QMatrix4x4 &newView)
{
    m_view = newView;
}

QSize Camera::viewportSize() const
{
    return m_viewportSize;
}

void Camera::setViewportSize(const QSize &newViewportSize)
{
    m_viewportSize = newViewportSize;
}

QVector3D Camera::position() const
{
    return m_position;
}

void Camera::setPosition(const QVector3D &newPosition)
{
    m_position = newPosition;
}

QVector3D Camera::viewVector() const
{
    return m_viewVector;
}

void Camera::setViewVector(const QVector3D &newViewVector)
{
    m_viewVector = newViewVector;
}
