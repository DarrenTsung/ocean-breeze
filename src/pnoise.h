#ifndef PERLIN_NOISE_H
#define PERLIN_NOISE_H

#include <Eigen/Core>

class PNoise {
    private:
        float amplitude, wavelength;

    public:
        // getters
        float get_amplitude();
        float get_wavelength();

        // setters
        void set_amplitude(float amp);
        void set_wavelength(float wav);

        // 2D - functions
        float get_height2D(float x, float y);
        Eigen::Vector2f get_gradient2D(int x, int y);

        // constructor
        PNoise() {}

};

#endif // PERLIN_NOISE_H
