#include <GLFW/glfw3.h>
#include <GLUT/glut.h>
#include <Eigen/Core>

#include <stdlib.h>
#include <stdio.h>

#include "app.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

const int GENERATED_WIDTH = 5;
const int GENERATED_HEIGHT = 5;

const float FRAMERATE = 60;

#define PI 3.14159265359

App application(GENERATED_WIDTH, GENERATED_HEIGHT);

void error_callback(int error, const char *description) {
    fputs(description, stderr);
}

// lots of reference was drawn from https://github.com/rodrigosetti/azteroids/
// thank you!
static void key_callback (GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
        return;
    }

    // set keys
    if (action == GLFW_PRESS || action == GLFW_RELEASE) {
        bool key_pressed = action == GLFW_PRESS;
        switch (key) {
            case GLFW_KEY_UP:
                application.keys_pressed.up    = key_pressed;
                break;
            case GLFW_KEY_DOWN:
                application.keys_pressed.down  = key_pressed;
                break;
            case GLFW_KEY_LEFT:
                application.keys_pressed.left  = key_pressed;
                break;
            case GLFW_KEY_RIGHT:
                application.keys_pressed.right = key_pressed;
                break;
            case GLFW_KEY_SPACE:
                application.keys_pressed.space = key_pressed;
                break;
            case GLFW_KEY_M:
                glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
                break;
            case GLFW_KEY_N:
                glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
                break;
        }
    }

    if (mods & GLFW_MOD_SHIFT) {
        application.keys_pressed.shift = true;
    } else {
        application.keys_pressed.shift = false;
    }

}

int main(int argc, char **argv) {
    GLFWwindow* window;
    double prevTime = 0.0;

    // MUST happen before glfwInit
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    // create the window
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Ocean-breeze", NULL, NULL);

    // if window failed to create, terminate glfw
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    application.initialize();

    application.setup_shaders();

    while (!glfwWindowShouldClose(window))
    {
        // run until user clicks exit on window
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        float ratio = width / (float) height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();
        glOrtho(-ratio, ratio, -1.5, 1.5, 10, -10);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        Eigen::Vector3f pos = application.camera_position;
        glTranslatef(pos[0], pos[1], pos[2]);
        glRotatef(application.camera_pitch, 1.0f, 0.0f, 0.0f);
        glRotatef(application.camera_roll, 0.0f, 0.0f, -1.0f);

        // get the time difference between the last time we ran the loop
        double delta = 0.0;
        double time;
        while (delta < 1.0f/FRAMERATE) {
            time = glfwGetTime();
            delta = time - prevTime;
        }
        prevTime = time;


        application.update(delta);
        application.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
