#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <QImage>
#include <QtMath>
#include <QVector2D>

class Heightmap {
public:
    Heightmap(const float& height, const QImage& img) : m_height(height), m_image(img) {}

    float Get(float x, float y) {
        // 实现双线性插值  Bilinear filter
        const QVector2D offset(-250, -250);
        const QVector2D dimensions(500, 500);

        const float xf = 1.0 - sat((x - offset.x()) / dimensions.x());
        const float yf = sat((y - offset.y()) / dimensions.y());
        const float w = m_image.width() - 1;
        const float h = m_image.height() - 1;

        const float x1 = qFloor(xf * w);
        const float y1 = qFloor(yf * h);
        const float x2 = clamp(x1 + 1, 0, w);
        const float y2 = clamp(y1 + 1, 0, h);

        const float xp = xf * w - x1;
        const float yp = yf * h - y1;

        const float p11 = GetPixelAsFloat(x1, y1);
        const float p21 = GetPixelAsFloat(x2, y1);
        const float p12 = GetPixelAsFloat(x1, y2);
        const float p22 = GetPixelAsFloat(x2, y2);

        const float px1 = lerp(xp, p11, p21);
        const float px2 = lerp(xp, p12, p22);

        return lerp(yp, px1, px2) * m_height;
    }

private:
    float GetPixelAsFloat(int x, int y) {
        QColor color = m_image.pixelColor(x, y);
        return color.red() / 255.0f;
    }

    float clamp(float x, float a, float b) {
        return qMin(qMax(x, a), b);
    }

    float sat(float x) {
        return qMin(qMax(x, 0.0), 1.0);
    }

    float lerp(float x, float a, float b) {
        return x * (b - a) + a;
    }

    float m_height;
    QImage m_image;
};

#endif // HEIGHTMAP_H
