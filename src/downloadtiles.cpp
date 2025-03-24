#include "downloadtiles.h"

DownLoadTiles::DownLoadTiles(QOpenGLContext *context, QObject *parent)
    : QObject(parent), m_stop(false), m_sharedContext(context)
{
    this->moveToThread(&m_thread);

    connect(&m_thread, &QThread::started, this, &DownLoadTiles::processTask);
    m_thread.start();
//    connect(m_thread, &QThread::finished, this, &DownloadThread::threadFinished);
}


DownLoadTiles::~DownLoadTiles()
{
    {
        QMutexLocker locker(&m_mutex);
        m_stop = true;
        m_condition.wakeAll();
    }
    m_thread.quit();
    m_thread.wait();
}

void DownLoadTiles::addTask(const mapStruct &task)
{
     QMutexLocker locker(&m_mutex);
     m_taskQueue.enqueue(task);
     m_condition.wakeOne();
}

#include <QOffscreenSurface>
void DownLoadTiles::processTask()
{
    // 创建子线程的 OpenGL 上下文
        QOpenGLContext context;
        context.setShareContext(m_sharedContext);
        if (!context.create()) {
            qWarning() << "Failed to create OpenGL context in thread.";
            return;
        }

        QSurfaceFormat format;
        format.setVersion(3, 3);
        format.setProfile(QSurfaceFormat::CoreProfile);
        context.setFormat(format);

        // 创建一个离屏表面
        QOffscreenSurface surface;
        surface.setFormat(context.format());
        surface.create();

        // 绑定上下文
        context.makeCurrent(&surface);
    QNetworkAccessManager manager;
    while (true) {
        mapStruct task;
        {
            QMutexLocker locker(&m_mutex);
            while (m_taskQueue.isEmpty() && !m_stop) {
                m_condition.wait(&m_mutex);
            }
            if (m_stop) {
                break;
            }
            task = m_taskQueue.dequeue();
        }

        // 下载
        QNetworkRequest request;
        request.setUrl(task.getUrl());
        QNetworkReply* reply = manager.get(request);

        QEventLoop loop;
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        if (reply->error() == QNetworkReply::NoError) {
//            qDebug()<<task.getUrl();
            auto data = reply->readAll();

            QImage img;
            img.loadFromData(data);
//            auto path = QString("/Users/fanglee/Desktop/tiles/%1-%2-%3.%4").arg(task.zoom).arg(task.x).arg(task.y).arg("jpg");
//            qDebug() << path;
//            img.save(path);
            if (!img.isNull()) {
                auto newimg = img.convertToFormat(QImage::Format_RGB888);
                qDebug() << "reply finish";
                task.texture->setData(0, task.layer, QOpenGLTexture::RGB, QOpenGLTexture::UInt8, newimg.constBits());
            }
        } else {
            qDebug() << "download img failed: " << reply->error();
        }
        reply->deleteLater();
    }

    context.doneCurrent();
}