#ifndef HEIGHTGENERATOR_H
#define HEIGHTGENERATOR_H
#include <QVector2D>
#include <QVector3D>

class Generator {
public:
    virtual QVector2D Get(float, float) = 0;
};

class HeightGenerator {
public:
    HeightGenerator(Generator* generator, const QVector2D& position, float minRadius, float maxRadius)
        : m_position(position), m_radius(minRadius, maxRadius), m_generator(generator) {}

    QVector3D Get(float x, float y) {
        float distance = (m_position - QVector2D(x, y)).length();
        float normalization = 1.0f - std::clamp((distance - m_radius.x()) / (m_radius.y() - m_radius.x()), 0.0f, 1.0f);
        normalization = normalization * normalization * (3.0f - 2.0f * normalization);
        return QVector3D(m_generator->Get(x, y), normalization);
    }

private:
    QVector2D m_position;
    QVector2D m_radius;
    Generator* m_generator;
};

class FlaredCornerHeightGenerator : public Generator {
public:
    QVector2D Get(float x, float y) override {
        if (x == -250 && y == 250) {
            return QVector2D(128, 1);
        }
        return QVector2D(0, 1);
    }
};

class BumpHeightGenerator : public Generator {
public:
    QVector2D Get(float x, float y) override {
        QVector2D pos(x, y);
        float dist = pos.length();
        float h = 1.0f - std::clamp(dist / 250.0f, 0.0f, 1.0f);
        h = h * h * h * (h * (h * 6 - 15) + 10);
        return QVector2D(h * 128, 1);
    }
};

#endif // HEIGHTGENERATOR_H
