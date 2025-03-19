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

    float heightFact() const;
    void setHeightFact(float newHeightFact);

signals:
    void cameraChanged();
    void iswireframeChanged();
    void updatePlane(int, int);

    void heightFactChanged();

private:
    Camera* m_camera = nullptr;
    Plane* m_plane = nullptr;
    float m_heightFact = 1.0;
    bool m_iswireframe = false;
    Q_PROPERTY(Camera *camera READ camera WRITE setCamera NOTIFY cameraChanged FINAL)
    Q_PROPERTY(bool wireframe READ iswireframe WRITE setIswireframe NOTIFY iswireframeChanged FINAL)
    Q_PROPERTY(float heightFact READ heightFact WRITE setHeightFact NOTIFY heightFactChanged FINAL)
};

#endif // GRAPHICS_H
