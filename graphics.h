#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <QQuickWindow>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QQuickFramebufferObject>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>


#include "camera.h"

class GraphicsRenderer : public QQuickFramebufferObject::Renderer
    , protected QOpenGLFunctions
{
public:
    GraphicsRenderer();
    ~GraphicsRenderer();

    void init();
    void paint();

    // Renderer interface
protected:
    virtual void render() override;
    virtual QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override;
    virtual void synchronize(QQuickFramebufferObject *) override;

private:
    QOpenGLShaderProgram *m_program = nullptr;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLTexture m_texture;
    QOpenGLBuffer m_vbo;
    QOpenGLBuffer m_ebo;
};

class Graphics : public QQuickFramebufferObject
{
    Q_OBJECT
    QML_ELEMENT
public:
    Graphics(QQuickItem *parent = nullptr);

    // QQuickFramebufferObject interface
public:
    virtual Renderer *createRenderer() const override;


    Camera *camera() const;
    void setCamera(Camera *newCamera);

signals:

    void cameraChanged();

private:
    Camera* m_camera;
    Q_PROPERTY(Camera *camera READ camera WRITE setCamera NOTIFY cameraChanged FINAL)
};

#endif // GRAPHICS_H
