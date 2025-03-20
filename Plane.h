#ifndef PLANE_H
#define PLANE_H

#include <QVector3D>
#include <QList>
#include <qdebug.h>

// #include "FastNoiseLite.h"

#include "src/NoiseGenerator.h"
#include "src/Heightmap.h"


struct Plane {
    Plane(int w, int h)
        : m_size(w, h)
        , m_heightmap(0, QImage())
    {
        generateVerticesAndIndices();
    }

    void setPlaneSize(int w, int h) {
        m_size = QSize(w, h);
        generateVerticesAndIndices();
    }
    void setNoisePerporties(float scale, float octaves, float persistence, float lacunarity
                           ,float exponentiation, float height, int noisetype) {
        m_noise.setNoisePerporties(scale, octaves, persistence, lacunarity, exponentiation, height, noisetype);
        generateVerticesAndIndices();
    }

    void setMapHeight(float height) {
        m_heightmap.setHeight(height);
        generateVerticesAndIndices();
    }
    void setMapImage(QImage image) {
        m_heightmap.setImage(image);
        generateVerticesAndIndices();
    }

    // 生成顶点坐标和索引
    void generateVerticesAndIndices() {
        // 清空现有的顶点和索引
        m_vertexs.clear();
        m_indices.clear();

        // 遍历每一行和每一列生成顶点
        for (int row = 0; row <= m_size.height(); ++row) {
            for (int col = 0; col <= m_size.width(); ++col) {
                float x = col;
                float z = row;
                // float y = m_noise.Get(x, z);
                float y = m_heightmap.Get(x, z);
                // qDebug() << y;
                m_vertexs.append(QVector3D(x, y, z));  // 平面上顶点的坐标 (x, 0, y)
            }
        }

        // 生成索引，绘制四边形
        for (int row = 0; row < m_size.height(); ++row) {
            for (int col = 0; col < m_size.width(); ++col) {
                uint topLeft = row * (m_size.width() + 1) + col;           // 当前四边形左上角
                uint bottomLeft = (row + 1) * (m_size.width() + 1) + col;   // 当前四边形左下角
                uint topRight = row * (m_size.width() + 1) + (col + 1);     // 当前四边形右上角
                uint bottomRight = (row + 1) * (m_size.width() + 1) + (col + 1); // 当前四边形右下角

                // 第一个三角形 (左上, 左下, 右上)
                m_indices.append(topLeft);
                m_indices.append(bottomLeft);
                m_indices.append(topRight);

                // 第二个三角形 (右上, 左下, 右下)
                m_indices.append(topRight);
                m_indices.append(bottomLeft);
                m_indices.append(bottomRight);
            }
        }
    }

    // 获取顶点坐标
    QList<QVector3D> GetVertexs() {
        return m_vertexs;
    }

    // 获取索引
    QList<uint> GetIndices() {
        return m_indices;
    }

    QList<QVector3D> m_vertexs;   // 顶点坐标
    QList<uint> m_indices;        // 绘制平面的索引
    QSize m_size;                 // 平面的宽高
    // FastNoiseLite m_noise;

    NoiseGenerator m_noise;
    Heightmap m_heightmap;
};

#endif // PLANE_H
