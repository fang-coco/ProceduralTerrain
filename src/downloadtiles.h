#ifndef DOWNLOADTILES_H
#define DOWNLOADTILES_H

#include <QThread>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <QImage>

#include <QNetworkAccessManager>

#include <QNetworkReply>
#include <QQuickFramebufferObject>
#include <QOpenGLTexture>
#include <QOpenGLContext>

struct mapStruct
{
    int x = 0;
    int y = 0;
    int zoom = 0;
    int layer = -1;
    QString format = "png";
    QString url = "http://wprd03.is.autonavi.com/appmaptile?style=6&x=%1&y=%2&z=%3";

    QOpenGLTexture* texture = nullptr;

    QUrl getUrl() {
        return url.arg(x).arg(y).arg(zoom);
    }
};

class DownLoadTiles : public QObject
{
    Q_OBJECT
public:
    explicit DownLoadTiles(QOpenGLContext* context, QObject *parent = nullptr);

    ~DownLoadTiles();

    void addTask(const mapStruct& task);

private:
    void processTask();

private:
    QMutex m_mutex;
    QWaitCondition m_condition;
    QQueue<mapStruct> m_taskQueue;
    bool m_stop = false;

    QThread m_thread;
    QOpenGLContext* m_sharedContext;

};

#endif // DOWNLOADTILES_H
