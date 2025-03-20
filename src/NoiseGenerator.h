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
        Simplex = 0,
        Perlin
    };

    void setNoisePerporties(float scale, float octaves, float persistence, float lacunarity
                           ,float exponentiation, float height, int noisetype) {
        m_scale = scale;
        m_octaves = octaves;
        m_persistence = persistence;
        m_lacunarity = lacunarity;
        m_exponentiation = exponentiation;
        m_height = height;
        m_noisetype = noisetype;
    }

private:
    float noise2D(float x, float y, int type) {
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
    float m_scale = 64.0;
    float m_octaves = 1;
    float m_persistence = 0.01;
    float m_lacunarity = 0.01;
    float m_exponentiation = 0.1;
    float m_height = 0;
    int m_noisetype = NoiseType::Simplex;
};

#endif // NOISEGENERATOR_H
