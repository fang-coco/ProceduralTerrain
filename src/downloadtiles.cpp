#include "downloadtiles.h"

DownLoadTiles::DownLoadTiles(QObject *parent)
    : QObject(parent), m_stop(false)
{
    this->moveToThread(&m_thread);
    m_thread.start();
    connect(&m_thread, &QThread::started, this, &DownLoadTiles::processTask);
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

void DownLoadTiles::processTask()
{
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
        auto reply = manager.get(request);

        QEventLoop loop;
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        if (reply->error() == QNetworkReply::NoError) {
            auto img = QImage(reply->read(256 * 256 * 4)).convertToFormat(QImage::Format_RGB888);
            if (!img.isNull()) {
                qDebug() << "reply finish";
                task.texture->setData(0, task.layer, QOpenGLTexture::RGB, QOpenGLTexture::UInt8, img.constBits());
            }
        } else {
            qDebug() << "download img failed: " << reply->error();
        }
        reply->deleteLater();
    }
}