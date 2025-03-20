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
#include "Plane.h"

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

    bool m_iswireframe;
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

    bool iswireframe() const;
    void setIswireframe(bool newIswireframe);

    Plane *plane() const;

signals:
    void cameraChanged();
    void iswireframeChanged();
    void updatePlaneSize(int, int);
    void updatePlaneNoise(float scale, float octaves, float persistence, float lacunarity
                           ,float exponentiation, float height, int noisetype);
    void updatePlaneMapHeight(float);
    void updatePlaneMapImage(QString);

private:
    Camera* m_camera = nullptr;
    Plane* m_plane = nullptr;
    bool m_iswireframe = false;
    Q_PROPERTY(Camera *camera READ camera WRITE setCamera NOTIFY cameraChanged FINAL)
    Q_PROPERTY(bool wireframe READ iswireframe WRITE setIswireframe NOTIFY iswireframeChanged FINAL)
};

#endif // GRAPHICS_H
