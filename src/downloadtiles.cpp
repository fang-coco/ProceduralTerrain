#include "downloadtiles.h"

DownLoadTiles::DownLoadTiles(QObject *parent)
    : QObject(parent), m_stop(false)
{
    this->moveToThread(&m_thread);
    connect(&m_thread, &QThread::started, this, &DownLoadTiles::processTask);
    m_thread.start();
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
                qDebug() << "reply finish";
                textureToLayer texture;
                texture.layerId = task.layer;
                texture.img = img.convertToFormat(QImage::Format_RGB888);
                texture.type = task.type;
                emit completeTask(texture);
            }
        } else {
            qDebug() << "download img failed: " << reply->error();
        }
        reply->deleteLater();
    }
}