#ifndef APP_H
#define APP_H

#include "pnoise.h"
#include <Eigen/Core>
#include <vector>

struct Keys {
    bool up    = false;
    bool down  = false;
    bool left  = false;
    bool right = false;
    bool space = false;
    bool shift = false;
};

class App {
    private:
        PNoise noise;
        int width, height;

        std::vector<std::vector<Eigen::Vector3f> > vertices;

    public:
        // struct for key input
        Keys keys_pressed;
        // degrees
        float camera_roll, camera_pitch;
        Eigen::Vector3f camera_position;

        // constructors
        App() {};
        App(int width, int height): width(width), height(height) {}

        // initialize the perlin noise
        void initialize();
        // update the application by delta time
        void update(double delta);
        // draws everything in the application
        void draw();
};

// log a formatted string to stderr
void log(const std::string fmt_str, ...);

#endif // APP_H
