#include "app.h"
#include "stdio.h"
#include <stdarg.h>  // for va_start, etc
#include <memory>    // for std::unique_ptr

#include <GLFW/glfw3.h> // for glVertex3f, etc
#include <Eigen/Geometry> // for cross product

using namespace Eigen;

void App::initialize() {
    // set up lights
    const GLfloat light_position0[] = { 0.0, 0.0, -10.0, 1.0 };
    const GLfloat light_diffuse0[] = { 0.1, 0.1, 0.1, 1.0 };

    const GLfloat light_position1[] = { 0.0, 0.0, 10.0, 1.0 };
    const GLfloat light_diffuse1[] = { 0.4, 0.4, 1.0, 1.0 };
    const GLfloat light_specular1[] = { 0.5, 0.5, 0.5, 1.0 };

    glClearColor (0.0, 0.0, 0.0, 0.0);
    glShadeModel (GL_FLAT);
    glEnable(GL_LIGHTING);

    glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular1);
    glEnable(GL_LIGHT1);

    glEnable(GL_DEPTH_TEST);

    // set up terrain
    noise.set_amplitude(3.0f);

    log("The amplitude is %g\n", noise.get_amplitude());

    for(float i=-width; i<=width+0.005; i+=0.1f) {
        std::vector<Eigen::Vector3f> row;
        for(float j=-height; j<=height+0.005; j+= 0.1f) {
            Vector3f vert(i, j, noise.get_height2D(i, j));
            //log("vert: (%f, %f, %f)\n", vert[0], vert[1], vert[2]);
            row.push_back(vert);
        }
        vertices.push_back(row);
    }

    // set up camera
    camera_roll = 30.0f;
    camera_pitch = 70.0f;

    camera_position = Vector3f(0, 0, 0);
}

void App::update(double delta) {
    float mvmt_speed = 20.0f;
    float cspeed = 0.7f;
    if (keys_pressed.up) {
        if (keys_pressed.shift) {
            camera_position += Vector3f(0, cspeed, 0)*delta;
        } else {
            camera_pitch += mvmt_speed*delta;
        }
    }
    if (keys_pressed.down) {
        if (keys_pressed.shift) {
            camera_position -= Vector3f(0, cspeed, 0)*delta;
        } else {
            camera_pitch -= mvmt_speed*delta;
        }
    }
    if (keys_pressed.left) {
        if (keys_pressed.shift) {
            camera_position -= Vector3f(cspeed, 0, 0)*delta;
        } else {
            camera_roll -= mvmt_speed*delta;
        }
    }
    if (keys_pressed.right) {
        if (keys_pressed.shift) {
            camera_position += Vector3f(cspeed, 0, 0)*delta;
        } else {
            camera_roll += mvmt_speed*delta;
        }
    }

}

void App::draw() {
    // draw each quad
    for(int i=0; i<vertices.size()-1; i++) {
        for(int j=0; j<vertices[i].size()-1; j++) {
            Vector3f bl = vertices[i][j];
            Vector3f br = vertices[i+1][j];
            Vector3f tl = vertices[i][j+1];
            Vector3f tr = vertices[i+1][j+1];

            Vector3f n0 = (br - bl).cross(tl - bl);
            n0.normalize();

            glBegin(GL_QUADS);
                glNormal3f(n0[0], n0[1], n0[2]);

                // counter-clockwise order
                glVertex3f(bl[0], bl[1], bl[2]);
                glVertex3f(br[0], br[1], br[2]);
                glVertex3f(tr[0], tr[1], tr[2]);
                glVertex3f(tl[0], tl[1], tl[2]);
            glEnd();
        }
    }

}

void log(const std::string fmt_str, ...) {
    int final_n, n = fmt_str.size() * 2; /* reserve 2 times as much as the length of the fmt_str */
    std::string str;
    std::unique_ptr<char[]> formatted;
    va_list ap;
    while(1) {
        formatted.reset(new char[n]); /* wrap the plain char array into the unique_ptr */
        strcpy(&formatted[0], fmt_str.c_str());
        va_start(ap, fmt_str);
        final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
        va_end(ap);
        if (final_n < 0 || final_n >= n)
            n += abs(final_n - n + 1);
        else
            break;
    }
    fputs(formatted.get(), stderr);
}
