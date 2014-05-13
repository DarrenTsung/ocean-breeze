#include "app.h"
#include "stdio.h"
#include <stdarg.h>  // for va_start, etc
#include <memory>    // for std::unique_ptr

#include <GLFW/glfw3.h> // for glVertex3f, etc
#include <Eigen/Geometry> // for cross product

#define PI 3.14159265359

using namespace Eigen;

void App::initialize() {
    // set up lights
    const GLfloat light_position0[] = { 0.0, 0.0, -10.0, 1.0 };
    const GLfloat light_diffuse0[] = { 0.1, 0.1, 0.1, 1.0 };

    const GLfloat light_position1[] = { 0.0, 0.0, 10.0, 1.0 };
    const GLfloat light_diffuse1[] = { 0.6, 0.6, 1.5, 1.0 };
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
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    // set up terrain
    noise.set_amplitude(2.0f);

    for(float i=-width; i<=width+0.005; i+=0.1f) {
        // verticies
        std::vector<Eigen::Vector3f> vrow;
        // normals of verticies
        std::vector<Eigen::Vector3f> nrow;

        for(float j=-height; j<=height+0.005; j+= 0.1f) {
            Vector3f vert(i, j, noise.get_height2D(i, j));
            //log("vert: (%f, %f, %f)\n", vert[0], vert[1], vert[2]);

            // compute the normal by getting partial derivatives numerically
            Vector3f vertup = Vector3f(i, j+0.0005, noise.get_height2D(i, j+0.0005)) - vert;
            Vector3f vertright = Vector3f(i+0.0005, j, noise.get_height2D(i+0.0005, j)) - vert;
            Vector3f norm = vertright.cross(vertup);
            norm.normalize();

            vrow.push_back(vert);
            nrow.push_back(norm);
        }
        vertices.push_back(vrow);
        normals.push_back(nrow);
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

    wave_angle += 10.0*delta;

    // shading angle uniform variable
    GLint loc = glGetUniformLocation(program, "angle");
    glUniform1f(loc, wave_angle*PI/180.0f);
}

void App::draw() {
    // draw each quad
    for(int i=0; i<vertices.size()-1; i++) {
        for(int j=0; j<vertices[i].size()-1; j++) {
            Vector3f bl = vertices[i][j];
            Vector3f br = vertices[i+1][j];
            Vector3f tl = vertices[i][j+1];
            Vector3f tr = vertices[i+1][j+1];

            Vector3f n_bl = normals[i][j];
            Vector3f n_br = normals[i+1][j];
            Vector3f n_tl = normals[i][j+1];
            Vector3f n_tr = normals[i+1][j+1];

            glBegin(GL_QUADS);
                glNormal3f(n_bl[0], n_bl[1], n_bl[2]);
                glVertex3f(bl[0], bl[1], bl[2]);

                glNormal3f(n_br[0], n_br[1], n_br[2]);
                glVertex3f(br[0], br[1], br[2]);

                glNormal3f(n_tr[0], n_tr[1], n_tr[2]);
                glVertex3f(tr[0], tr[1], tr[2]);

                glNormal3f(n_tl[0], n_tl[1], n_tl[2]);
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

std::string App::loadFileToString(char const * const fname)
{
    std::ifstream ifile(fname);
    std::string filetext;

    while( ifile.good() ) {
        std::string line;
        std::getline(ifile, line);
        filetext.append(line + "\n");
    }

    return filetext;
}

void App::setup_shaders() {
    // create the program object outside so the rest of the program can reference it
    program = glCreateProgram();

    //// create the vertex shader
    if (true) {
        // grab the source of the vshader
        std::string vshader_string = loadFileToString("src/vshader1.vert");
        GLchar const *vshader_source = vshader_string.c_str();
        GLint const vshader_length = vshader_string.size();
        // grab the source of the fshader
        std::string fshader_string = loadFileToString("src/fshader1.frag");
        GLchar const *fshader_source = fshader_string.c_str();
        GLint const fshader_length = fshader_string.size();

        // create the shader object
        GLenum vertex_shader;
        GLenum fragment_shader;
        vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

        // load in the source shader code
        glShaderSource(vertex_shader, 1, &vshader_source, &vshader_length);
        glShaderSource(fragment_shader, 1, &fshader_source, &fshader_length);

        // compile the shader code
        glCompileShader(vertex_shader);
        // check that compiled correctly
        GLint isCompiled = 0;
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &isCompiled);
        if(isCompiled == GL_FALSE)
        {
                GLint maxLength = 0;
                glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &maxLength);

                //The maxLength includes the NULL character
                std::vector<char> errorLog(maxLength);
                glGetShaderInfoLog(vertex_shader, maxLength, &maxLength, &errorLog[0]);

                //Provide the infolog in whatever manor you deem best.
                for(int i=0;i<maxLength; i++) {
                    std::cout << errorLog[i];
                }
                std::cout << std::endl;
                //Exit with failure.
                glDeleteShader(vertex_shader); //Don't leak the shader.
        }
        glCompileShader(fragment_shader);

        GLint compiled;

        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compiled);
        if (compiled == GL_FALSE) {
            GLint maxLength = 0;
            glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &maxLength);

            //The maxLength includes the NULL character
            std::vector<char> errorLog(maxLength);
            glGetShaderInfoLog(fragment_shader, maxLength, &maxLength, &errorLog[0]);

            //Provide the infolog in whatever manor you deem best.
            for(int i=0;i<maxLength; i++) {
                std::cout << errorLog[i];
            }
            std::cout << std::endl;
            //Exit with failure.
            glDeleteShader(fragment_shader); //Don't leak the shader.
        } else {
            std::cout << "compiled!" << std::endl;
            // attach shader to program
            glAttachShader(program, vertex_shader);
            glAttachShader(program, fragment_shader);

            // link and use program object
            glLinkProgram(program);

            GLint linked;
            glGetProgramiv(program, GL_LINK_STATUS, &linked);
            if (linked)
            {
                std::cout << "shaders linked!" << std::endl;
                glUseProgram(program);
            }
            else
            {
                std::cout << "shaders not linked.." << std::endl;
                GLint maxLength = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

                //The maxLength includes the NULL character
                std::vector<GLchar> infoLog(maxLength);
                glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

                //We don't need the program anymore.
                glDeleteProgram(program);
                //Don't leak shaders either.
                glDeleteShader(vertex_shader);

                //Use the infoLog as you see fit.
                for(int i=0;i<maxLength; i++) {
                    std::cout << infoLog[i];
                }
                std::cout << std::endl;
            }
        }
    }
}
