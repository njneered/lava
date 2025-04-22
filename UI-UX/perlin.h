/*
 * Basic Perlin noise implementation
 * Adapted from various public domain implementations
 */

#ifndef PERLIN_HPP
#define PERLIN_HPP

#include <cmath>
#include <random>
#include <vector>
#include <algorithm>

namespace siv {

class PerlinNoise {
private:
    std::vector<std::int32_t> p;

    typedef std::uint32_t seed_type;

    static double fade(double t) noexcept {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    static double lerp(double t, double a, double b) noexcept {
        return a + t * (b - a);
    }

    static double grad(std::int32_t hash, double x, double y, double z) noexcept {
        const std::int32_t h = hash & 15;
        const double u = h < 8 ? x : y;
        const double v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
        return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
    }

public:
    explicit PerlinNoise(seed_type seed = std::default_random_engine::default_seed) {
        reseed(seed);
    }

    void reseed(seed_type seed) {
        p.resize(512);

        // Initialize with values 0-255
        std::iota(p.begin(), p.begin() + 256, 0);

        // Shuffle using the seed
        std::mt19937 engine(seed);
        std::shuffle(p.begin(), p.begin() + 256, engine);

        // Duplicate the array
        std::copy_n(p.begin(), 256, p.begin() + 256);
    }

    // 3D Perlin noise
    double noise3D(double x, double y, double z) const noexcept {
        const std::int32_t X = static_cast<std::int32_t>(std::floor(x)) & 255;
        const std::int32_t Y = static_cast<std::int32_t>(std::floor(y)) & 255;
        const std::int32_t Z = static_cast<std::int32_t>(std::floor(z)) & 255;

        x -= std::floor(x);
        y -= std::floor(y);
        z -= std::floor(z);

        const double u = fade(x);
        const double v = fade(y);
        const double w = fade(z);

        const int A  = p[X] + Y;
        const int AA = p[A] + Z;
        const int AB = p[A + 1] + Z;
        const int B  = p[X + 1] + Y;
        const int BA = p[B] + Z;
        const int BB = p[B + 1] + Z;

        return lerp(w, lerp(v, lerp(u, grad(p[AA], x, y, z),
                                       grad(p[BA], x - 1, y, z)),
                               lerp(u, grad(p[AB], x, y - 1, z),
                                       grad(p[BB], x - 1, y - 1, z))),
                       lerp(v, lerp(u, grad(p[AA + 1], x, y, z - 1),
                                       grad(p[BA + 1], x - 1, y, z - 1)),
                               lerp(u, grad(p[AB + 1], x, y - 1, z - 1),
                                       grad(p[BB + 1], x - 1, y - 1, z - 1))));
    }

    // 2D Perlin noise
    double noise2D(double x, double y) const noexcept {
        return noise3D(x, y, 0);
    }

    // 1D Perlin noise
    double noise1D(double x) const noexcept {
        return noise3D(x, 0, 0);
    }

    // Normalized 2D noise (0.0 to 1.0)
    double normalizedNoise2D(double x, double y) const noexcept {
        return (noise2D(x, y) + 1.0) / 2.0;
    }

    // Octave noise for more natural patterns
    double octaveNoise2D(double x, double y, int octaves, double persistence) const noexcept {
        double total = 0.0;
        double frequency = 1.0;
        double amplitude = 1.0;
        double maxValue = 0.0;

        for (int i = 0; i < octaves; i++) {
            total += normalizedNoise2D(x * frequency, y * frequency) * amplitude;
            maxValue += amplitude;
            amplitude *= persistence;
            frequency *= 2;
        }

        return total / maxValue;
    }

    // Normalized octave noise (0.0 to 1.0)
    double normalizedOctave2D_01(double x, double y, int octaves, double persistence) const noexcept {
        return octaveNoise2D(x, y, octaves, persistence);
    }
};

} // namespace siv

#endif // PERLIN_HPP