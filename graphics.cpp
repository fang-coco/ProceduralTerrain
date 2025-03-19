#include "graphics.h"
#include <QtCore/QRunnable>
#include <QOpenGLFramebufferObjectFormat>

static const int MAX_TEXTURE_COUNT = 1024;
static const QSize TEXTURE_SIZE = QSize(256, 256);

Graphics::Graphics(QQuickItem *parent)
    : QQuickFramebufferObject(parent)
    , m_plane(new Plane(256, 256))
{
    connect(this, &Graphics::cameraChanged, this, [this](){
        this->update();
    });

    connect(this, &Graphics::iswireframeChanged, this, [this](){
        this->update();
    });
    connect(this, &Graphics::updatePlane, this, [this](int w, int h){
        this->m_plane->setPlaneSize(w, h);
        this->update();
    });
    connect(this, &Graphics::heightFactChanged, this, [this](){
        this->update();
    });
}

Camera *Graphics::camera() const
{
    return m_camera;
}

void Graphics::setCamera(Camera *newCamera)
{
    // if (m_camera == newCamera)
    //     return;
    m_camera = newCamera;
    emit cameraChanged();
}

bool Graphics::iswireframe() const
{
    return m_iswireframe;
}

void Graphics::setIswireframe(bool newIswireframe)
{
    if (m_iswireframe == newIswireframe)
        return;
    m_iswireframe = newIswireframe;
    emit iswireframeChanged();
}

Plane *Graphics::plane() const
{
    return m_plane;
}

float Graphics::heightFact() const
{
    return m_heightFact;
}

void Graphics::setHeightFact(float newHeightFact)
{
    if (qFuzzyCompare(m_heightFact, newHeightFact))
        return;
    m_heightFact = newHeightFact;
    emit heightFactChanged();
}

QQuickFramebufferObject::Renderer *Graphics::createRenderer() const
{
    return new GraphicsRenderer;
}


GraphicsRenderer::GraphicsRenderer()
    : m_texture(QOpenGLTexture(QOpenGLTexture::Target2DArray))
    , m_vbo(QOpenGLBuffer(QOpenGLBuffer::VertexBuffer))
    , m_ebo(QOpenGLBuffer(QOpenGLBuffer::IndexBuffer))

{
    init();
}

GraphicsRenderer::~GraphicsRenderer()
{
    delete m_program;
}

QOpenGLFramebufferObject *GraphicsRenderer::createFramebufferObject(const QSize &size)
{
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    return new QOpenGLFramebufferObject(size, format);
}

void GraphicsRenderer::init()
{
    if (!m_program) {
        initializeOpenGLFunctions();
        m_program = new QOpenGLShaderProgram();

        QString shadersDir = "/Users/fanglee/Project/ProceduralTerrain/shaders/";
        m_program->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex,   shadersDir + "plane.vert");
        m_program->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, shadersDir + "plane.frag");

        m_program->bindAttributeLocation("position", 0);
        m_program->bindAttributeLocation("texcoord", 1);
        m_program->link();
    }
    if (!m_texture.isCreated()) {
        m_texture.create();
        m_texture.setLayers(MAX_TEXTURE_COUNT);
        m_texture.setSize(TEXTURE_SIZE.width(), TEXTURE_SIZE.height());
        m_texture.setFormat(QOpenGLTexture::RGBA8_UNorm);
        m_texture.allocateStorage();
    }

    if (!m_vao.isCreated()) m_vao.create();
    if (!m_vbo.isCreated()) m_vbo.create();
    if (!m_ebo.isCreated()) m_ebo.create();
}


void GraphicsRenderer::render()
{
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.2f, 0.2f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 设置多边形模式为线框模式，这样只会绘制多边形的边而不是填充它们
    if (m_iswireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    m_program->bind();
    m_texture.bind(0);
    m_vao.bind();
    m_vbo.bind();
    m_ebo.bind();
    paint();
    m_ebo.release();
    m_vbo.release();
    m_vao.release();
    m_texture.release(0);
    m_program->release();
}

void GraphicsRenderer::synchronize(QQuickFramebufferObject * item)
{
    auto obj = reinterpret_cast<Graphics*>(item);
    // Setting
    {
        m_iswireframe =  obj->iswireframe();
    }

    // float vertexs[] = {
    //     -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0,
    //     -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0,
    //      0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0,
    //      0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0,
    // };

    auto plane = obj->plane();
    auto vertexs = plane->GetVertexs();
    m_vbo.bind();
    m_vbo.allocate(vertexs.data(), vertexs.size() * sizeof(QVector3D));
    m_vbo.release();

    // ushort indexs[] = {
    //     0, 1, 2, 3, 1, 2
    // };
    auto indexs = plane->GetIndices();

    m_ebo.bind();
    m_ebo.allocate(indexs.data(), sizeof(uint) * indexs.size());
    m_ebo.release();

    QString texturesDirPath = "/Users/fanglee/Project/ProceduralTerrain/textures/";
    QImage img;
    img.load(texturesDirPath + "833.jpg");
    img.mirror();
    auto newimg = img.convertToFormat(QImage::Format_RGB888);
    m_texture.setData(0, 0, QOpenGLTexture::RGB, QOpenGLTexture::UInt8, newimg.constBits());

    auto camera = reinterpret_cast<Graphics*>(item)->camera();
    m_program->bind();
    QMatrix4x4 model;
    model.setToIdentity();
    m_program->setUniformValue("model", model);
    m_program->setUniformValue("projection", camera->projection());
    m_program->setUniformValue("view", camera->view());
    m_program->setUniformValue("step", (float)1.0 / plane->m_size.width());
    m_program->setUniformValue("heightFact", obj->heightFact());
    m_program->release();

    // qDebug() << camera->projection()
    //          << camera->view()
    //          << camera->position();

}


void GraphicsRenderer::paint()
{
    // qDebug() << "painting ...";

    glActiveTexture(GL_TEXTURE0);

    int stride = 3 * sizeof(float);
    m_program->enableAttributeArray(0);
    // m_program->enableAttributeArray(1);

    m_program->setAttributeBuffer(0, GL_FLOAT, 0, 3, stride);
    // m_program->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(float), 3, stride);

    m_program->setUniformValue("tex", 0);
    // qDebug() << m_ebo.size() / sizeof(ushort);
    glDrawElements(GL_TRIANGLES, m_ebo.size() / sizeof(uint), GL_UNSIGNED_INT, nullptr);
    // glDrawArrays(GL_LINES, 0, 8);
    m_program->disableAttributeArray(0);
    m_program->disableAttributeArray(1);
}
