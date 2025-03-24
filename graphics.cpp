#include "graphics.h"
#include <QtCore/QRunnable>
#include <QOpenGLFramebufferObjectFormat>


struct TextureMappingInfo
{
    // float layer, float scale, vec2 offset
    QVector4D layerScaleOffset[4]; // 4 * 16
    QVector4D extentMinMax[4]; // 4 * 16
};
static_assert(sizeof(TextureMappingInfo) == 8 * 16, "TextureInfo doesn't satisfy std140 layout");

// For now we assume you have a GL 4.5 capable gpu
const int maxUniformBlockSize = 65536; // 16384 is what the standard mandates but recent gpu support 4 times more
const int maxTextureMappingInfoCount = maxUniformBlockSize / sizeof(TextureMappingInfo);// 65536 / 128 -> 512

static const int MAX_LAYER_COUNT = 1024 * 2;
static const QSize TEXTURE_SIZE = QSize(256, 256);

Graphics::Graphics(QQuickItem *parent)
    : QQuickFramebufferObject(parent)
    , m_camera(new Camera)
    , m_builder(new CubeBuilder(5, 15, m_camera))
{
    connect(this, &Graphics::updateCamera
          , this, [this](float fov, QSize viewportSize
                       , float clipNear, float clipFar
                       , QVector3D position, QVector3D front
                       , QVector3D up) {
        // qDebug() << "camera update";
        m_camera->updateCameraProperties(fov, viewportSize, clipNear, clipFar, position, front, up);
        m_builder->update();
        this->update();
    });
    connect(this, &Graphics::useImageryChanged, this, [this](){
        this->update();
    });
    connect(this, &Graphics::useElevationChanged, this, [this](){
        this->update();
    });
    connect(this, &Graphics::useWireFrameChanged, this, [this](){
        this->update();
        qDebug() << "wireframe change";
    });
    connect(this, &Graphics::subdivisionChanged, this, [this](){
        this->update();
        qDebug() << "subdivision change: " << m_subdivision;
    });
}

Camera *Graphics::camera() const
{
    return m_camera;
}

CubeBuilder *Graphics::builder() const
{
    return m_builder;
}

bool Graphics::useElevation() const
{
    return m_useElevation;
}

void Graphics::setUseElevation(bool newUseElevation)
{
    if (m_useElevation == newUseElevation)
        return;
    m_useElevation = newUseElevation;
    emit useElevationChanged();
}

bool Graphics::useImagery() const
{
    return m_useImagery;
}

void Graphics::setUseImagery(bool newUseImagery)
{
    if (m_useImagery == newUseImagery)
        return;
    m_useImagery = newUseImagery;
    emit useImageryChanged();
}

bool Graphics::useWireFrame() const
{
    return m_useWireFrame;
}

void Graphics::setUseWireFrame(bool newUseWireFrame)
{
    if (m_useWireFrame == newUseWireFrame)
        return;
    m_useWireFrame = newUseWireFrame;
    emit useWireFrameChanged();
}

int Graphics::subdivision() const
{
    return m_subdivision;
}

void Graphics::setSubdivision(int newSubdivision)
{
    if (m_subdivision == newSubdivision)
        return;
    m_subdivision = newSubdivision;
    emit subdivisionChanged();
}

void GraphicsRenderer::updateImageryScores()
{
    for (ImageEntryToLayer &e : m_imageEntriesForSatellite)
        --e.score;
    for (ImageEntryToLayer &e : m_imageEntriesForElevation)
        --e.score;
}

int GraphicsRenderer::satelliteTextureLayerFor(int zoom, quint64 x, quint64 y, CubeBuilder* builder)
{
    int matchingIdx = -1;
    int nearestUnused = -1;
    int idx = 0;

    // Find if we have a matching layer
    for (const ImageEntryToLayer &e : m_imageEntriesForSatellite) {
        if (nearestUnused ==  -1 && e.score < 0)
            nearestUnused = idx;
        if (e.score >= 0 && e.tileX == x && e.tileY == y && e.zoom == zoom) {
            matchingIdx = idx;
            break;
        }
        ++idx;
    }

    // If we have a matching idx, use it
    if (matchingIdx != -1) {
        ImageEntryToLayer &e = m_imageEntriesForSatellite[matchingIdx];
        // Reset score
        e.score = 1;
        return e.layerId;
    }

    // Otherwise if we've found an unused block
    if (nearestUnused != -1 ) {
        // We are dealing with a block that was never used before
        ImageEntryToLayer &e = m_imageEntriesForSatellite[nearestUnused];
        if (e.layerId == -1) {
            // Try to create a new QTextureImage for the texture2DArray
            if (m_currImageryLayer + 1 < MAX_LAYER_COUNT) {
                e.layerId = m_currImageryLayer++;
            }
            else {
                return 0;
            }
        }
        e.score = 1;
        e.zoom = zoom;
        e.tileX = x;
        e.tileY = y;

        mapStruct tile;
        tile.layer = e.layerId;
        tile.x = e.tileX;
        tile.y = e.tileY;
        tile.zoom = e.zoom;
        tile.texture = &m_satelliteTexture;
        m_threads->addTask(tile);
//        QImage img = builder->imageryTiles(ImageryTileProvider::Satellite, zoom, x, y);
//        auto newimg = img.convertToFormat(QImage::Format_RGB888);
//        m_satelliteTexture.setData(0, e.layerId, QOpenGLTexture::RGB, QOpenGLTexture::UInt8, newimg.constBits());
        return e.layerId;

    }
    return 0;
}

int GraphicsRenderer::elevationTextureLayerFor(int zoom, quint64 x, quint64 y, CubeBuilder * builder)
{
    int matchingIdx = -1;
    int nearestUnused = -1;
    int idx = 0;

    // Find if we have a matching layer
    for (const ImageEntryToLayer &e : m_imageEntriesForElevation) {
        if (nearestUnused ==  -1 && e.score < 0)
            nearestUnused = idx;
        if (e.score >= 0 && e.tileX == x && e.tileY == y && e.zoom == zoom) {
            matchingIdx = idx;
            break;
        }
        ++idx;
    }

    // If we have a matching idx, use it
    if (matchingIdx != -1) {
        ImageEntryToLayer &e = m_imageEntriesForElevation[matchingIdx];
        // Reset score
        e.score = 1;
        return e.layerId;
    }

    // Otherwise if we've found an unused block
    if (nearestUnused != -1 ) {
        // We are dealing with a block that was never used before
        ImageEntryToLayer &e = m_imageEntriesForElevation[nearestUnused];
        if (e.layerId == -1) {
            // Try to create a new QTextureImage for the texture2DArray
            if (m_currElevationLayer + 1 < MAX_LAYER_COUNT) {
                e.layerId = m_currElevationLayer++;
            }
            else {
                return 0;
            }
        }

        e.score = 1;
        e.zoom = zoom;
        e.tileX = x;
        e.tileY = y;
        QImage img = builder->imageryTiles(ImageryTileProvider::Elevation, zoom, x, y);
        auto newimg = img.convertToFormat(QImage::Format_RGB888);
        m_elevationTexture.setData(0, e.layerId, QOpenGLTexture::RGB, QOpenGLTexture::UInt8, newimg.constBits());
        return e.layerId;
    }
    return 0;
}

QQuickFramebufferObject::Renderer *Graphics::createRenderer() const
{
    return new GraphicsRenderer;
}

GraphicsRenderer::GraphicsRenderer()
    : m_satelliteTexture(QOpenGLTexture(QOpenGLTexture::Target2DArray))
    , m_elevationTexture(QOpenGLTexture(QOpenGLTexture::Target2DArray))
    , m_imageEntriesForElevation(maxTextureMappingInfoCount)
    , m_imageEntriesForSatellite(maxTextureMappingInfoCount)
    , m_vbo(QOpenGLBuffer(QOpenGLBuffer::VertexBuffer))
{
    QOpenGLContext mainContext;
    mainContext.create();
    m_threads.reset(new DownLoadTiles(&mainContext));
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
        m_program->addShaderFromSourceFile(QOpenGLShader::Vertex,   shadersDir + "cubesphere.vert");
        m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, shadersDir + "cubesphere.frag");
        m_program->addShaderFromSourceFile(QOpenGLShader::Geometry, shadersDir + "cubesphere.geom");
        m_program->addShaderFromSourceFile(QOpenGLShader::TessellationControl,    shadersDir + "cubesphere.tc");
        m_program->addShaderFromSourceFile(QOpenGLShader::TessellationEvaluation, shadersDir + "cubesphere.te");

        m_program->bindAttributeLocation("position", 0);
        m_program->link();
    }

    // ubo
    {
        glGenBuffers(1, &m_satelliteUbo);
        glBindBuffer(GL_UNIFORM_BUFFER, m_satelliteUbo);
        glBufferData(GL_UNIFORM_BUFFER, 512 * 128, NULL, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        GLuint satellite_index = glGetUniformBlockIndex(m_program->programId(), "satelliteUniformBuffer");
        glUniformBlockBinding(m_program->programId(), satellite_index, 1);

        glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_satelliteUbo);

        glGenBuffers(1, &m_elevationUbo);
        glBindBuffer(GL_UNIFORM_BUFFER, m_elevationUbo);
        glBufferData(GL_UNIFORM_BUFFER, 512 * 128, NULL, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        GLuint elevation_index = glGetUniformBlockIndex(m_program->programId(), "elevationUniformBuffer");
        glUniformBlockBinding(m_program->programId(), elevation_index, 2);

        glBindBufferBase(GL_UNIFORM_BUFFER, 2, m_elevationUbo);
    }



    if (!m_satelliteTexture.isCreated()) {
         m_satelliteTexture.create();
         m_satelliteTexture.setLayers(MAX_LAYER_COUNT);
         m_satelliteTexture.setSize(TEXTURE_SIZE.width(), TEXTURE_SIZE.height());
         m_satelliteTexture.setFormat(QOpenGLTexture::RGBA8_UNorm);
         m_satelliteTexture.allocateStorage();
    }

        if (!m_elevationTexture.isCreated()) {
         m_elevationTexture.create();
         m_elevationTexture.setLayers(MAX_LAYER_COUNT);
         m_elevationTexture.setSize(TEXTURE_SIZE.width(), TEXTURE_SIZE.height());
         m_elevationTexture.setFormat(QOpenGLTexture::RGBA8_UNorm);
         m_elevationTexture.allocateStorage();
    }

    if (!m_vao.isCreated()) m_vao.create();
    if (!m_vbo.isCreated()) m_vbo.create();
}


void GraphicsRenderer::render()
{
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 设置多边形模式为线框模式，这样只会绘制多边形的边而不是填充它们
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    m_program->bind();
    m_satelliteTexture.bind(0);
    m_elevationTexture.bind(1);
    m_vao.bind();
    m_vbo.bind();
    paint();
    m_vbo.release();
    m_vao.release();
    m_satelliteTexture.release(0);
    m_elevationTexture.release(1);
    m_program->release();
}

void GraphicsRenderer::synchronize(QQuickFramebufferObject * item)
{
    auto obj = reinterpret_cast<Graphics*>(item);
    auto camera = reinterpret_cast<Graphics*>(item)->camera();
    auto builder = reinterpret_cast<Graphics*>(item)->builder();

    // uniform
    {
        m_program->bind();
        m_program->setUniformValue("satelliteImagery", 0);
        m_program->setUniformValue("elevationImagery", 1);
        m_program->setUniformValue("projection", camera->projection());
        m_program->setUniformValue("view", camera->view());
        m_program->setUniformValue("subdivision", obj->subdivision());
        m_program->setUniformValue("useImagery", obj->useImagery());
        m_program->setUniformValue("useElevation", obj->useElevation());
        m_program->setUniformValue("useWireFrame", obj->useWireFrame());
        m_program->release();
    }

    // SPHERE VERTICES
    {
        // Copy vertex data to buffers
        const QVector<QDoubleVector3D> vertices = builder->sphereVertices();
        const QDoubleVector3D eyePos = camera->position();
        QVector<QVector4D> verticesRelativeToEye(vertices.size());

        int i = 0;
        // We transform each world vertex so that it is relative to eye
        for (const QDoubleVector3D &vec3d : vertices) {
            const QDoubleVector3D rte = vec3d - eyePos;
            // We slightly abuse from the 4th component of the vertex
            // to store the vertex entry into the UBO
            const int indexIntoUBO = i / 4; // Same index for all the vertices of a same tile
            const QVector4D rteVertexData(rte.toQVector3D(), indexIntoUBO);
            verticesRelativeToEye[i++] = rteVertexData;
        }

        QByteArray vertexData;
        const int vertexBufferByteSize = vertices.size() * sizeof(QVector4D);
        vertexData.resize(vertexBufferByteSize);
        memcpy(vertexData.data(), verticesRelativeToEye.constData(), vertexBufferByteSize);

        m_vbo.bind();
        m_vbo.allocate(vertexData.data(), vertexData.size());
        m_vbo.release();

        // Each tile has 4 corners, tessellation shaders will convert that to 8x8 grids
        const int patchesCount = vertices.size() / 4;
        qDebug() << patchesCount;
    }


    // uniform block buffer
    {
        // // 测试细分着色器
        // float vertex[3] = {0.0, 0.0, 0.0};
        // m_vbo.bind();
        // m_vbo.allocate(vertex, 3 * sizeof(float));
        // m_vbo.release();
        // qDebug() << camera->projection()
        //          << camera->view()
        //          << camera->position();

        m_program->bind();
        if (obj->useImagery()) {
            QByteArray uboData;
            uboData.resize(maxUniformBlockSize);
            TextureMappingInfo *textureMappings = reinterpret_cast<TextureMappingInfo *>(uboData.data());
            const QVector<TileImageryMapping> mappings = builder->tileImageMappings(ImageryTileProvider::Satellite);

            // 1 Tile Imagery Mapping contains up to 4 ImageryMappings
            // Each ImageryMapping contains
            // offsetXY to textureCoords
            // extentXY
            // scaleXY
            // textureLayer
            for (int i = 0, m = qMin(maxTextureMappingInfoCount, mappings.size()); i < m; ++i) {
                const TileImageryMapping &mapping = mappings.at(i);
                for (int j = 0; j < 4; ++j) {
                    const ImageryMapping &vertexMapping = mapping.mappingEntries[j];
                    if (vertexMapping.zoom == -1) {
                         textureMappings[i].layerScaleOffset[j][0] = -1.0f;
                    } else {
                        const float layer = satelliteTextureLayerFor(vertexMapping.zoom,
                                                                     vertexMapping.tileX,
                                                                     vertexMapping.tileY, builder);
                        textureMappings[i].layerScaleOffset[j] = QVector4D(layer,
                                                                            vertexMapping.scale,
                                                                            vertexMapping.offsetX,
                                                                            vertexMapping.offsetY);
                        textureMappings[i].extentMinMax[j] = QVector4D(vertexMapping.extentXMin,
                                                                       vertexMapping.extentYMin,
                                                                       vertexMapping.extentXMax,
                                                                       vertexMapping.extentYMax);
                    }
                }
            }

            glBindBuffer(GL_UNIFORM_BUFFER, m_satelliteUbo);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, maxUniformBlockSize, uboData.constData());
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }

        // Perform UBO update for Elevation Imagery
        if (m_useElevation) {
            QByteArray uboData;
            uboData.resize(maxUniformBlockSize);
            TextureMappingInfo *textureMappings = reinterpret_cast<TextureMappingInfo *>(uboData.data());
            const QVector<TileImageryMapping> mappings = builder->tileImageMappings(ImageryTileProvider::Elevation);

            // 1 Tile Imagery Mapping contains up to 4 ImageryMappings
            // Each ImageryMapping contains
            // offsetXY to textureCoords
            // extentXY
            // scaleXY
            // textureLayer
            for (int i = 0, m = qMin(maxTextureMappingInfoCount, mappings.size()); i < m; ++i) {
                const TileImageryMapping &mapping = mappings.at(i);
                for (int j = 0; j < 4; ++j) {
                    const ImageryMapping &vertexMapping = mapping.mappingEntries[j];
                    if (vertexMapping.zoom == -1) {
                         textureMappings[i].layerScaleOffset[j][0] = -1.0f;
                    } else {
                        const float layer = elevationTextureLayerFor(vertexMapping.zoom,
                                                                     vertexMapping.tileX,
                                                                     vertexMapping.tileY, builder);
                        textureMappings[i].layerScaleOffset[j] = QVector4D(layer,
                                                                            vertexMapping.scale,
                                                                            vertexMapping.offsetX,
                                                                            vertexMapping.offsetY);
                        textureMappings[i].extentMinMax[j] = QVector4D(vertexMapping.extentXMin,
                                                                       vertexMapping.extentYMin,
                                                                       vertexMapping.extentXMax,
                                                                       vertexMapping.extentYMax);
                    }
                }
            }
            glBindBuffer(GL_UNIFORM_BUFFER, m_elevationUbo);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, maxUniformBlockSize, uboData.constData());
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }
        m_program->release();
    }

    updateImageryScores();
}


void GraphicsRenderer::paint()
{
    // qDebug() << "painting ...";

     glActiveTexture(GL_TEXTURE0);
     glActiveTexture(GL_TEXTURE1);

    int stride = 4 * sizeof(float);
    m_program->enableAttributeArray(0);

    m_program->setAttributeBuffer(0, GL_FLOAT, 0, 4, stride);

    glPatchParameteri(GL_PATCH_VERTICES, 4);
    glDrawArrays(GL_PATCHES, 0, m_vbo.size() / sizeof(QVector4D));
    m_program->disableAttributeArray(0);
}
