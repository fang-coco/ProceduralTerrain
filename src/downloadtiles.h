#ifndef DOWNLOADTILES_H
#define DOWNLOADTILES_H

#include <QThread>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <QImage>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>

struct textureToLayer
{
    int layerId = -1;
    int type;
    QImage img;
};

struct mapStruct
{
    int x = 0;
    int y = 0;
    int zoom = 0;
    int layer = -1;

    int type = 1; // 1 --> sate、2 --> elev
    QString url = "http://wprd03.is.autonavi.com/appmaptile?style=6&x=%1&y=%2&z=%3";


    QUrl getUrl() {
        return url.arg(x).arg(y).arg(zoom);
    }
};

class DownLoadTiles : public QObject
{
    Q_OBJECT
public:
    explicit DownLoadTiles(QObject *parent = nullptr);

    ~DownLoadTiles();
    void addTask(const mapStruct& task);

private:
    void processTask();

signals:
    void completeTask(const textureToLayer);

private:
    QMutex m_mutex;
    QWaitCondition m_condition;
    QQueue<mapStruct> m_taskQueue;
    bool m_stop = false;

    QThread m_thread;
};

#endif // DOWNLOADTILES_H
