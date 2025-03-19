#ifndef NOISEGENERATOR_H
#define NOISEGENERATOR_H
#include <FastNoiseLite.h>
#include <QtMath>

class NoiseGenerator {
public:
    float Get(float x, float y) {
        const float xs = x / m_scale;
        const float ys = y / m_scale;
        float amplitude = 1.0;
        float frequency = 1.0;
        float normalization = 0;
        float total = 0;
        for (int o = 0; o < m_octaves; o++) {
            float noiseValue = noise2D(xs * frequency, ys * frequency, m_noisetype) * 0.5 + 0.5;
            total += noiseValue * amplitude;
            normalization += amplitude;
            amplitude *= m_persistence;
            frequency *= m_lacunarity;
        }
        total /= normalization;
        return qPow(total, m_exponentiation) * m_height;
    }

    enum NoiseType {
        Simplex,
        Perlin
    };

private:
    float noise2D(int x, int y, int type) {
        FastNoiseLite noise;
        if (type == NoiseType::Perlin) {
            noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
            float value = noise.GetNoise(x, y);
            return value * 2.0 - 1.0;
        } else {
            noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
            return noise.GetNoise(x, y);
        }
    }

public:
    float m_scale;
    float m_octaves;
    float m_persistence;
    float m_lacunarity;
    float m_exponentiation;
    float m_height;
    int m_noisetype;
};

#endif // NOISEGENERATOR_H
