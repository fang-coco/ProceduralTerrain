#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <QQuickWindow>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QQuickFramebufferObject>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QObject>
#include <QOpenGLTexture>


#include "camera.h"

#include "cubesphere/cubebuilder.h"
#include "src/downloadtiles.h"

class GraphicsRenderer : public QQuickFramebufferObject::Renderer
    , protected QOpenGLFunctions
{
public:
    GraphicsRenderer();
    ~GraphicsRenderer();

    void init();
    void paint();

    struct ImageEntryToLayer
    {
        int zoom = 0;
        int score = -1;
        int layerId = -1;
        quint64 tileX = 0;
        quint64 tileY = 0;
    };


    void updateImageryScores();

    // Renderer interface
    int satelliteTextureLayerFor(int zoom, quint64 x, quint64 y, CubeBuilder *builder);
    int elevationTextureLayerFor(int zoom, quint64 x, quint64 y, CubeBuilder *builder);
protected:
    virtual void render() override;
    virtual QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override;
    virtual void synchronize(QQuickFramebufferObject *) override;

private:
    QOpenGLShaderProgram *m_program = nullptr;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vbo;
    QOpenGLTexture m_satelliteTexture;
    QOpenGLTexture m_elevationTexture;
    GLuint m_satelliteUbo;
    GLuint m_elevationUbo;

    bool m_useImagery;
    bool m_useElevation;
    int m_currImageryLayer;
    int m_currElevationLayer;

    QVector<ImageEntryToLayer> m_imageEntriesForSatellite;
    QVector<ImageEntryToLayer> m_imageEntriesForElevation;


    QScopedPointer<DownLoadTiles> m_threads;
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
    CubeBuilder *builder() const;


    bool useElevation() const;
    void setUseElevation(bool newUseElevation);

    bool useImagery() const;
    void setUseImagery(bool newUseImagery);

    bool useWireFrame() const;
    void setUseWireFrame(bool newUseWireFrame);

    int subdivision() const;
    void setSubdivision(int newSubdivision);

signals:
    void updateCamera(float fov, QSize viewportSize
                    , float clipNear, float clipFar
                    , QVector3D position, QVector3D front
                    , QVector3D up);

    void useElevationChanged();

    void useImageryChanged();

    void useWireFrameChanged();

    void subdivisionChanged();

private:
    Camera* m_camera = nullptr;
    CubeBuilder* m_builder = nullptr;

    int m_subdivision;
    bool m_useImagery;
    bool m_useElevation;
    bool m_useWireFrame;

    Q_PROPERTY(bool useElevation READ useElevation WRITE setUseElevation NOTIFY useElevationChanged FINAL)
    Q_PROPERTY(bool useImagery READ useImagery WRITE setUseImagery NOTIFY useImageryChanged FINAL)
    Q_PROPERTY(bool useWireFrame READ useWireFrame WRITE setUseWireFrame NOTIFY useWireFrameChanged FINAL)
    Q_PROPERTY(int subdivision READ subdivision WRITE setSubdivision NOTIFY subdivisionChanged FINAL)
};

#endif // GRAPHICS_H
