#include "pnoise.h"
#include <math.h>
#include "app.h"

using namespace Eigen;

float PNoise::get_height2D(float x, float y) {
    // the four points around the point x, y
    int x0, x1, y0, y1;

    // computed the points around the input
    x0 = (int)floor(x);
    y0 = (int)floor(y);
    x1 = x0 + 1;
    y1 = y0 + 1;

    // the gradient vectors (top-left, top-right, bottom-left, bottom-right)
    // the coordinate system has y going up as it increments
    Vector2f tl = get_gradient2D(x0, y1);
    Vector2f tr = get_gradient2D(x1, y1);
    Vector2f bl = get_gradient2D(x0, y0);
    Vector2f br = get_gradient2D(x1, y0);

    // the dot product of the vectors from each point to the input point to the
    // pseudo-random gradients of each point
    // http://webstaff.itn.liu.se/~stegu/TNM022-2005/perlinnoiselinks/perlin-noise-math-faq.html
    float s = bl.dot(Vector2f(x, y) - Vector2f(x0, y0));
    float t = br.dot(Vector2f(x, y) - Vector2f(x1, y0));
    float u = tl.dot(Vector2f(x, y) - Vector2f(x0, y1));
    float v = tr.dot(Vector2f(x, y) - Vector2f(x1, y1));

    //log("%f, %f, %f, %f\n", s, t, u, v);

    // we use the easing cure 3p^2 - 2p^3
    // Sx is the weighted average for x components, we use it on s and t since they have the same x
    float Sx = 3*pow((x - x0), 2) - 2*pow((x - x0), 3);
    // a is the final weighted value after averaging s and t
    float a = s + Sx*(t - s);
    // b is the final weighted value after averaging u and v
    float b = u + Sx*(v - u);

    //log("Sx: %g, %g, %g\n", Sx, a, b);

    // now we find our weighted average for y components
    float Sy = 3*pow((y - y0), 2) - 2*pow((y - y0), 3);
    // find weighted average of a and b
    float z = a + Sy*(b - a);

    //log("Sy: %g, z:%g\n", Sy, z);

    return z;
}

Vector2f PNoise::get_gradient2D(int x, int y) {
    // seed is the same given the same x and y
    float seed = 1234567*x + 4321*y;
    srand(seed);

    // x0 and x1 are floats between [0, amplitude)
    float x0 = (((float)rand() / RAND_MAX) * amplitude) - (amplitude/2);
    float x1 = (((float)rand() / RAND_MAX) * amplitude) - (amplitude/2);

    return Vector2f(x0, x1);
}

// getters
float PNoise::get_amplitude() {
    return amplitude;
}

float PNoise::get_wavelength() {
    return wavelength;
}

// setters
void PNoise::set_amplitude(float amp) {
    amplitude = amp;
}

void PNoise::set_wavelength(float wav) {
    wavelength = wav;
}
