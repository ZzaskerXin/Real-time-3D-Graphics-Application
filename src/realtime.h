#pragma once

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>

#include "shapes/Cone.h"
#include "shapes/Cube.h"
#include "shapes/Cylinder.h"
#include "shapes/Sphere.h"
#include "utils/sceneparser.h"
#include "settings.h"
#include "src/utils/shaderloader.h"
#include "debug.h"

class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    void sceneChanged();
    void settingsChanged();
    void saveViewportImage(std::string filePath);

    RenderData data;

    GLuint m_defaultFBO;
    int m_fbo_width;
    int m_fbo_height;
    int m_screen_width;
    int m_screen_height;

    GLuint m_texture_shader;
    GLuint m_fullscreen_vbo;
    GLuint m_fullscreen_vao;
    QImage m_image;
    GLuint m_kitten_texture;
    GLuint m_fbo;
    GLuint m_fbo_texture;
    GLuint m_fbo_renderbuffer;

    GLuint m_shader;
    GLuint m_postprocessing_shader;

    GLuint m_spherevbo;
    GLuint m_spherevao;

    GLuint m_conevbo;
    GLuint m_conevao;

    GLuint m_cubevbo;
    GLuint m_cubevao;

    GLuint m_cylindervbo;
    GLuint m_cylindervao;

    Sphere sphere;
    Cone cone;
    Cube cube;
    Cylinder cylinder;

    std::vector<float> m_sphereData;
    std::vector<float> m_coneData;
    std::vector<float> m_cubeData;
    std::vector<float> m_cylinderData;

    float aspectR;

    glm::mat4 m_model = glm::mat4(1);
    glm::mat4 m_view  = glm::mat4(1);
    glm::mat4 m_proj  = glm::mat4(1);

    float m_ka;
    float m_kd;
    float m_ks;
    float m_shininess;

    // // float m_angleX = 6;
    // // float m_angleY = 0;
    // // float m_zoom = 2;

    void makeFBO(){
        // Task 19: Generate and bind an empty texture, set its min/mag filter interpolation, then unbind
        glGenTextures(1, &m_fbo_texture);
        Debug::glErrorCheck();
        glActiveTexture(GL_TEXTURE0);
        Debug::glErrorCheck();
        glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
        Debug::glErrorCheck();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_fbo_width, m_fbo_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        Debug::glErrorCheck();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        Debug::glErrorCheck();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        Debug::glErrorCheck();
        glBindTexture(GL_TEXTURE_2D, 0);
        Debug::glErrorCheck();

        glGenRenderbuffers(1, &m_fbo_renderbuffer);
        Debug::glErrorCheck();
        glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_renderbuffer);
        Debug::glErrorCheck();
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_fbo_width, m_fbo_height);
        Debug::glErrorCheck();
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        Debug::glErrorCheck();

        glGenFramebuffers(1, &m_fbo);
        Debug::glErrorCheck();
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        Debug::glErrorCheck();

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_texture, 0);
        Debug::glErrorCheck();
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_fbo_renderbuffer);
        Debug::glErrorCheck();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        Debug::glErrorCheck();
    }

    void paintTexture(GLuint texture, bool pixel, bool kernel){
        glUseProgram(m_postprocessing_shader);
        Debug::glErrorCheck();

        GLint pixelLoc = glGetUniformLocation(m_postprocessing_shader, "pixel");
        Debug::glErrorCheck();
        glUniform1i(pixelLoc, pixel ? 1 : 0);
        Debug::glErrorCheck();

        GLint kernelLoc = glGetUniformLocation(m_postprocessing_shader, "kernel");
        Debug::glErrorCheck();
        glUniform1i(kernelLoc, kernel ? 1 : 0);
        Debug::glErrorCheck();

        glBindVertexArray(m_fullscreen_vao);
        Debug::glErrorCheck();

        glActiveTexture(GL_TEXTURE0);
        Debug::glErrorCheck();
        glBindTexture(GL_TEXTURE_2D, texture);
        Debug::glErrorCheck();

        glDrawArrays(GL_TRIANGLES, 0, 6);
        Debug::glErrorCheck();
        glBindTexture(GL_TEXTURE_2D, 0);
        Debug::glErrorCheck();
        glBindVertexArray(0);
        Debug::glErrorCheck();
        glUseProgram(0);
        Debug::glErrorCheck();
    }

    void updateshapeData() {
        for (RenderShapeData &shape : data.shapes) {
            if (shape.primitive.type == PrimitiveType::PRIMITIVE_SPHERE) {
                sphere.updateParams(settings.shapeParameter1, settings.shapeParameter2);
                m_sphereData = sphere.generateShape();
            } else if (shape.primitive.type == PrimitiveType::PRIMITIVE_CONE) {
                cone.updateParams(settings.shapeParameter1, settings.shapeParameter2);
                m_coneData = cone.generateShape();
            } else if (shape.primitive.type == PrimitiveType::PRIMITIVE_CUBE) {
                cube.updateParams(settings.shapeParameter1);
                m_cubeData = cube.generateShape();
            } else if (shape.primitive.type == PrimitiveType::PRIMITIVE_CYLINDER) {
                cylinder.updateParams(settings.shapeParameter1, settings.shapeParameter2);
                m_cylinderData = cylinder.generateShape();
            }
        }
    }

    void uploadShapeData() {
        this->makeCurrent();
        //Sphere
        if (m_sphereData.size() != 0) {
            glBindVertexArray(m_spherevao);
            Debug::glErrorCheck();

            glBindBuffer(GL_ARRAY_BUFFER, m_spherevbo);
            Debug::glErrorCheck();

            glBufferData(GL_ARRAY_BUFFER, m_sphereData.size() * sizeof(float), m_sphereData.data(), GL_STATIC_DRAW);
            Debug::glErrorCheck();

            glEnableVertexAttribArray(0);
            Debug::glErrorCheck();

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
            Debug::glErrorCheck();

            glEnableVertexAttribArray(1);
            Debug::glErrorCheck();

            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
            Debug::glErrorCheck();

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            Debug::glErrorCheck();
            glBindVertexArray(0);
            Debug::glErrorCheck();
        }

        //Cone
        if (m_coneData.size() != 0) {
            glBindVertexArray(m_conevao);
            Debug::glErrorCheck();

            glBindBuffer(GL_ARRAY_BUFFER, m_conevbo);
            Debug::glErrorCheck();

            glBufferData(GL_ARRAY_BUFFER, m_coneData.size() * sizeof(float), m_coneData.data(), GL_STATIC_DRAW);
            Debug::glErrorCheck();

            glEnableVertexAttribArray(0);
            Debug::glErrorCheck();

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
            Debug::glErrorCheck();

            glEnableVertexAttribArray(1);
            Debug::glErrorCheck();

            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
            Debug::glErrorCheck();

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            Debug::glErrorCheck();
            glBindVertexArray(0);
            Debug::glErrorCheck();
        }

        // //Cube
        if (m_cubeData.size() != 0) {
            glBindVertexArray(m_cubevao);
            Debug::glErrorCheck();

            glBindBuffer(GL_ARRAY_BUFFER, m_cubevbo);
            Debug::glErrorCheck();

            glBufferData(GL_ARRAY_BUFFER, m_cubeData.size() * sizeof(float), m_cubeData.data(), GL_STATIC_DRAW);
            Debug::glErrorCheck();

            glEnableVertexAttribArray(0);
            Debug::glErrorCheck();

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
            Debug::glErrorCheck();

            glEnableVertexAttribArray(1);
            Debug::glErrorCheck();

            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
            Debug::glErrorCheck();

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            Debug::glErrorCheck();
            glBindVertexArray(0);
            Debug::glErrorCheck();
        }

        // //Cylinder
        if (m_cylinderData.size() != 0) {
            glBindVertexArray(m_cylindervao);
            Debug::glErrorCheck();

            glBindBuffer(GL_ARRAY_BUFFER, m_cylindervbo);
            Debug::glErrorCheck();

            glBufferData(GL_ARRAY_BUFFER, m_cylinderData.size() * sizeof(float), m_cylinderData.data(), GL_STATIC_DRAW);
            Debug::glErrorCheck();

            glEnableVertexAttribArray(0);
            Debug::glErrorCheck();

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
            Debug::glErrorCheck();

            glEnableVertexAttribArray(1);
            Debug::glErrorCheck();

            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
            Debug::glErrorCheck();

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            Debug::glErrorCheck();
            glBindVertexArray(0);
            Debug::glErrorCheck();
        }
        this->doneCurrent();
    }

    void paint() {

        GLint kaLoc = glGetUniformLocation(m_shader, "k_a");
        glUniform1f(kaLoc, m_ka);

        GLint kdLoc = glGetUniformLocation(m_shader, "k_d");
        glUniform1f(kdLoc, m_kd);

        GLint ksLoc = glGetUniformLocation(m_shader, "k_s");
        glUniform1f(ksLoc, m_ks);

        glm::mat4 inverseView = glm::inverse(m_view);
        glm::vec4 cameraPosition = inverseView * glm::vec4(0.0, 0.0, 0.0, 1.0);
        GLint viewPosLoc = glGetUniformLocation(m_shader, "viewPosition");
        Debug::glErrorCheck();
        glUniform4fv(viewPosLoc, 1, &cameraPosition[0]);
        Debug::glErrorCheck();


        GLint viewLoc = glGetUniformLocation(m_shader, "viewMatrix");
        Debug::glErrorCheck();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &m_view[0][0]);
        Debug::glErrorCheck();

        GLint projLoc = glGetUniformLocation(m_shader, "projectionMatrix");
        Debug::glErrorCheck();
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, &m_proj[0][0]);
        Debug::glErrorCheck();

        int directionallightcount = 0;
        int pointlightcount = 0;
        int spotlightcount = 0;

        int directionalid = 0;
        int pointid = 0;
        int spotid = 0;

        int LightCount = data.lights.size();

        for (int i = 0; i < LightCount; ++i) {
            if (data.lights[i].type == LightType::LIGHT_DIRECTIONAL) {

                std::string directionName = "directionalLightDirections[" + std::to_string(directionalid) + "]";
                GLint directionLoc = glGetUniformLocation(m_shader, directionName.c_str());
                Debug::glErrorCheck();
                glUniform3f(directionLoc, data.lights[i].dir[0], data.lights[i].dir[1], data.lights[i].dir[2]);
                Debug::glErrorCheck();

                std::string colorName = "directionalLightColors[" + std::to_string(directionalid) + "]";
                GLint colorLoc = glGetUniformLocation(m_shader, colorName.c_str());
                Debug::glErrorCheck();
                glUniform3f(colorLoc, data.lights[i].color[0], data.lights[i].color[1], data.lights[i].color[2]);
                Debug::glErrorCheck();

                directionallightcount += 1;
                directionalid += 1;
            }

            if (data.lights[i].type == LightType::LIGHT_POINT) {

                std::string posName = "pointLightPos[" + std::to_string(pointid) + "]";
                GLint posLoc = glGetUniformLocation(m_shader, posName.c_str());
                Debug::glErrorCheck();
                glUniform3f(posLoc, data.lights[i].pos[0], data.lights[i].pos[1], data.lights[i].pos[2]);
                Debug::glErrorCheck();

                std::string colorName = "pointLightColors[" + std::to_string(pointid) + "]";
                GLint colorLoc = glGetUniformLocation(m_shader, colorName.c_str());
                Debug::glErrorCheck();
                glUniform3f(colorLoc, data.lights[i].color[0], data.lights[i].color[1], data.lights[i].color[2]);
                Debug::glErrorCheck();

                std::string FunctionName = "pointFunction[" + std::to_string(pointid) + "]";
                GLint FunctionLoc = glGetUniformLocation(m_shader, FunctionName.c_str());
                Debug::glErrorCheck();
                glUniform3f(FunctionLoc, data.lights[i].function[0], data.lights[i].function[1], data.lights[i].function[2]);
                Debug::glErrorCheck();

                pointlightcount += 1;
                pointid += 1;
            }

            if (data.lights[i].type == LightType::LIGHT_SPOT) {

                std::string directionName = "spotLightDirections[" + std::to_string(spotid) + "]";
                GLint directionLoc = glGetUniformLocation(m_shader, directionName.c_str());
                Debug::glErrorCheck();
                glUniform3f(directionLoc, data.lights[i].dir[0], data.lights[i].dir[1], data.lights[i].dir[2]);
                Debug::glErrorCheck();

                std::string posName = "spotLightPos[" + std::to_string(spotid) + "]";
                GLint posLoc = glGetUniformLocation(m_shader, posName.c_str());
                Debug::glErrorCheck();
                glUniform3f(posLoc, data.lights[i].pos[0], data.lights[i].pos[1], data.lights[i].pos[2]);
                Debug::glErrorCheck();

                std::string colorName = "spotLightColors[" + std::to_string(spotid) + "]";
                GLint colorLoc = glGetUniformLocation(m_shader, colorName.c_str());
                Debug::glErrorCheck();
                glUniform3f(colorLoc, data.lights[i].color[0], data.lights[i].color[1], data.lights[i].color[2]);
                Debug::glErrorCheck();

                std::string FunctionName = "spotFunction[" + std::to_string(spotid) + "]";
                GLint FunctionLoc = glGetUniformLocation(m_shader, FunctionName.c_str());
                Debug::glErrorCheck();
                glUniform3f(FunctionLoc, data.lights[i].function[0], data.lights[i].function[1], data.lights[i].function[2]);
                std::cout << data.lights[i].function[0] << " " << data.lights[i].function[1] << " " << data.lights[i].function[2] << std::endl;
                Debug::glErrorCheck();

                std::string penumbraName = "penumbra[" + std::to_string(spotid) + "]";
                GLint penumbraLoc = glGetUniformLocation(m_shader, penumbraName.c_str());
                Debug::glErrorCheck();
                glUniform1f(penumbraLoc, data.lights[i].penumbra);
                Debug::glErrorCheck();

                std::string angleName = "angle[" + std::to_string(spotid) + "]";
                GLint angleLoc = glGetUniformLocation(m_shader, angleName.c_str());
                Debug::glErrorCheck();
                glUniform1f(angleLoc, data.lights[i].angle);
                std::cout << data.lights[i].angle << std::endl;
                Debug::glErrorCheck();

                spotlightcount += 1;
                spotid += 1;
            }
        }

        GLint numDirectionalLoc = glGetUniformLocation(m_shader, "numDirectionalLights");
        Debug::glErrorCheck();
        glUniform1i(numDirectionalLoc, directionallightcount);
        Debug::glErrorCheck();

        GLint numPointLoc = glGetUniformLocation(m_shader, "numPointLights");
        Debug::glErrorCheck();
        glUniform1i(numPointLoc, pointlightcount);
        Debug::glErrorCheck();

        GLint numSpotLoc = glGetUniformLocation(m_shader, "numSpotLights");
        Debug::glErrorCheck();
        glUniform1i(numSpotLoc, spotlightcount);
        Debug::glErrorCheck();

        for (RenderShapeData &shape : data.shapes) {

            m_model = shape.ctm;

            m_shininess = shape.primitive.material.shininess;

            glm::vec3 ca = glm::vec3(shape.primitive.material.cAmbient[0], shape.primitive.material.cAmbient[1], shape.primitive.material.cAmbient[2]);
            glm::vec3 cd = glm::vec3(shape.primitive.material.cDiffuse[0], shape.primitive.material.cDiffuse[1], shape.primitive.material.cDiffuse[2]);
            glm::vec3 cs = glm::vec3(shape.primitive.material.cSpecular[0], shape.primitive.material.cSpecular[1], shape.primitive.material.cSpecular[2]);

            GLint ambientLoc = glGetUniformLocation(m_shader, "ambient");
            GLint diffuseLoc = glGetUniformLocation(m_shader, "diffuse");
            GLint specularLoc = glGetUniformLocation(m_shader, "specular");

            glUniform3fv(ambientLoc, 1, &ca[0]);
            glUniform3fv(diffuseLoc, 1, &cd[0]);
            glUniform3fv(specularLoc, 1, &cs[0]);

            GLint shininessLoc = glGetUniformLocation(m_shader, "shininess");
            glUniform1f(shininessLoc, m_shininess);

            GLint modelLoc = glGetUniformLocation(m_shader, "modelMatrix");
            Debug::glErrorCheck();
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &m_model[0][0]);
            Debug::glErrorCheck();

            if (shape.primitive.type == PrimitiveType::PRIMITIVE_SPHERE) {
                glBindVertexArray(m_spherevao);
                Debug::glErrorCheck();
                glDrawArrays(GL_TRIANGLES, 0, m_sphereData.size() / 6);
                Debug::glErrorCheck();
            }
            else if (shape.primitive.type == PrimitiveType::PRIMITIVE_CONE) {
                glBindVertexArray(m_conevao);
                Debug::glErrorCheck();
                glDrawArrays(GL_TRIANGLES, 0, m_coneData.size() / 6);
                Debug::glErrorCheck();
            }
            else if (shape.primitive.type == PrimitiveType::PRIMITIVE_CUBE) {
                glBindVertexArray(m_cubevao);
                Debug::glErrorCheck();
                glDrawArrays(GL_TRIANGLES, 0, m_cubeData.size() / 6);
                Debug::glErrorCheck();
            }
            else if (shape.primitive.type == PrimitiveType::PRIMITIVE_CYLINDER) {
                glBindVertexArray(m_cylindervao);
                Debug::glErrorCheck();
                glDrawArrays(GL_TRIANGLES, 0, m_cylinderData.size() / 6);
                Debug::glErrorCheck();
            }
            glBindVertexArray(0);
        }
    }

public slots:
    void tick(QTimerEvent* event);                      // Called once per tick of m_timer

protected:
    void initializeGL() override;                       // Called once at the start of the program
    void paintGL() override;                            // Called whenever the OpenGL context changes or by an update() request
    void resizeGL(int width, int height) override;      // Called when window size changes

private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames

    // Input Related Variables
    bool m_mouseDown = false;                           // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not

    // Device Correction Variables
    double m_devicePixelRatio;
};
