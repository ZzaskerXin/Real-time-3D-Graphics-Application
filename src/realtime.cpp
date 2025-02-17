#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "camera/camera.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "settings.h"


Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_prev_mouse_pos = glm::vec2(size().width()/2, size().height()/2);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W]       = false;
    m_keyMap[Qt::Key_A]       = false;
    m_keyMap[Qt::Key_S]       = false;
    m_keyMap[Qt::Key_D]       = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space]   = false;

}

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    glDeleteBuffers(1, &m_spherevbo);
    glDeleteVertexArrays(1, &m_spherevao);

    glDeleteBuffers(1, &m_conevbo);
    glDeleteVertexArrays(1, &m_conevao);

    glDeleteBuffers(1, &m_cubevbo);
    glDeleteVertexArrays(1, &m_cubevao);

    glDeleteBuffers(1, &m_cylindervbo);
    glDeleteVertexArrays(1, &m_cylindervao);

// p6
    glDeleteBuffers(1, &m_fullscreen_vbo);
    glDeleteVertexArrays(1, &m_fullscreen_vao);

    glDeleteTextures(1, &m_fbo_texture);
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);


    glDeleteProgram(m_postprocessing_shader);
//P6
    glDeleteProgram(m_shader);

    this->doneCurrent();
}

void Realtime::initializeGL() {
    m_devicePixelRatio = this->devicePixelRatio();

    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();

//P6
    m_defaultFBO = 2;
    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_fbo_width = m_screen_width;
    m_fbo_height = m_screen_height;
//P6

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
    glEnable(GL_CULL_FACE);
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Students: anything requiring OpenGL calls when the program starts should be done here
    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert", ":/resources/shaders/default.frag");

//P6
    m_postprocessing_shader = ShaderLoader::createShaderProgram(":/resources/shaders/postprocessing.vert", ":/resources/shaders/postprocessing.frag");

    glUseProgram(m_postprocessing_shader);
    Debug::glErrorCheck();
    GLint textureLoc = glGetUniformLocation(m_postprocessing_shader, "t");
    Debug::glErrorCheck();
    glUniform1i(textureLoc, 0);
    Debug::glErrorCheck();
    glUseProgram(0);
    Debug::glErrorCheck();

    std::vector<GLfloat> fullscreen_quad_data =
        {
            -1.0f,  1.0f, 0.0f,   0.0f, 1.0f, // Top-left corner
            -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, // Bottom-left corner
            1.0f, -1.0f, 0.0f,   1.0f, 0.0f, // Bottom-right corner
            1.0f,  1.0f, 0.0f,   1.0f, 1.0f, // Top-right corner
            -1.0f,  1.0f, 0.0f,   0.0f, 1.0f, // Top-left corner
            1.0f, -1.0f, 0.0f,   1.0f, 0.0f  // Bottom-right corner
        };

    glGenBuffers(1, &m_fullscreen_vbo);
    Debug::glErrorCheck();
    glBindBuffer(GL_ARRAY_BUFFER, m_fullscreen_vbo);
    Debug::glErrorCheck();
    glBufferData(GL_ARRAY_BUFFER, fullscreen_quad_data.size()*sizeof(GLfloat), fullscreen_quad_data.data(), GL_STATIC_DRAW);
    Debug::glErrorCheck();
    glGenVertexArrays(1, &m_fullscreen_vao);
    Debug::glErrorCheck();
    glBindVertexArray(m_fullscreen_vao);
    Debug::glErrorCheck();


    glEnableVertexAttribArray(0);
    Debug::glErrorCheck();
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);
    Debug::glErrorCheck();
    glEnableVertexAttribArray(1);
    Debug::glErrorCheck();
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    Debug::glErrorCheck();


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    Debug::glErrorCheck();
    glBindVertexArray(0);
    Debug::glErrorCheck();

    makeFBO();
//P6

    glGenBuffers(1, &m_spherevbo);
    Debug::glErrorCheck();
    glGenVertexArrays(1, &m_spherevao);
    Debug::glErrorCheck();

    glGenBuffers(1, &m_cubevbo);
    Debug::glErrorCheck();
    glGenVertexArrays(1, &m_cubevao);
    Debug::glErrorCheck();

    glGenBuffers(1, &m_conevbo);
    Debug::glErrorCheck();
    glGenVertexArrays(1, &m_conevao);
    Debug::glErrorCheck();

    glGenBuffers(1, &m_cylindervbo);
    Debug::glErrorCheck();
    glGenVertexArrays(1, &m_cylindervao);
    Debug::glErrorCheck();
}

void Realtime::paintGL() {
    // Students: anything requiring OpenGL calls every frame should be done here
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    Debug::glErrorCheck();

    glViewport(0, 0, m_fbo_width, m_fbo_height);
    Debug::glErrorCheck();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Debug::glErrorCheck();

    glUseProgram(m_shader);
    Debug::glErrorCheck();

    paint();

    glUseProgram(0);
    Debug::glErrorCheck();

    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
    Debug::glErrorCheck();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Debug::glErrorCheck();

    paintTexture(m_fbo_texture, settings.perPixelFilter, settings.kernelBasedFilter);
}

void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

// P6
    glDeleteTextures(1, &m_fbo_texture);
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);

    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_fbo_width = m_screen_width;
    m_fbo_height = m_screen_height;

    makeFBO();
// P6


    // Students: anything requiring OpenGL calls when the program starts should be done here
    m_proj = glm::perspective(glm::radians(45.0), 1.0 * w / h, double(settings.nearPlane), double(settings.farPlane));
}

void Realtime::sceneChanged() {
    data = RenderData();
    SceneParser::parse(settings.sceneFilePath, data);
    Camera camera = Camera(data.cameraData);
    int h = height();
    int w = width();

    // aspectR = float(w) / float(h);

    // float tanHalfFov = tan(data.cameraData.heightAngle / 2.0f);



    // m_proj[0][0] = 1.0f / (aspectR * tanHalfFov);
    // m_proj[1][1] = 1.0f / tanHalfFov;
    // m_proj[2][2] = -(settings.farPlane + settings.nearPlane) / (settings.farPlane - settings.nearPlane);
    // m_proj[2][3] = -1.0f;
    // m_proj[3][2] = -(2.0f * settings.farPlane * settings.nearPlane) / (settings.farPlane - settings.nearPlane);

    m_proj = camera.getProjMatrix(h, w);
    m_view = camera.getViewMatrix();
    m_ka = data.globalData.ka;
    m_kd = data.globalData.kd;
    m_ks = data.globalData.ks;
    updateshapeData();
    uploadShapeData();
    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged() {

    updateshapeData();
    if (settings.sceneFilePath != "") {
        uploadShapeData();
    }
    Camera camera = Camera(data.cameraData);
    int h = height();
    int w = width();
    m_proj = camera.getProjMatrix(h, w);

    // aspectR = float(w) / float(h);

    // float tanHalfFov = tan(data.cameraData.heightAngle / 2.0f);



    // m_proj[0][0] = 1.0f / (aspectR * tanHalfFov);
    // m_proj[1][1] = 1.0f / tanHalfFov;
    // m_proj[2][2] = -(settings.farPlane + settings.nearPlane) / (settings.farPlane - settings.nearPlane);
    // m_proj[2][3] = -1.0f;
    // m_proj[3][2] = -(2.0f * settings.farPlane * settings.nearPlane) / (settings.farPlane - settings.nearPlane);

    update(); // asks for a PaintGL() call to occur
}

// ================== Project 6: Action!

void Realtime::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
}

void Realtime::keyReleaseEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = false;
}

void Realtime::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = true;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }
}

void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Use deltaX and deltaY here to rotate

        float sensitivity = 0.1f;

        // Calculate the rotation angles
        float yawAngle = glm::radians(deltaX * sensitivity);
        float pitchAngle = glm::radians(deltaY * sensitivity);

        // Get the current camera vectors
        glm::vec3 look = glm::normalize(glm::vec3(data.cameraData.look));
        glm::vec3 up = glm::normalize(glm::vec3(data.cameraData.up));
        glm::vec3 right = glm::normalize(glm::vec3(glm::cross(look, up)));

        // Compute the yaw rotation matrix (around the world Y-axis)
        glm::mat3 yawRotation = glm::mat3(
            cos(yawAngle), 0, sin(yawAngle),
            0, 1, 0,
            -sin(yawAngle), 0, cos(yawAngle)
        );

        // Rotate the look vector and right vector using the yaw rotation
        look = yawRotation * look;
        right = yawRotation * right;

        // Compute the pitch rotation matrix (around the camera's right vector)
        float cosPitch = cos(pitchAngle);
        float sinPitch = sin(pitchAngle);

        glm::mat3 pitchRotation = glm::mat3(
            cosPitch + right.x * right.x * (1 - cosPitch), right.x * right.y * (1 - cosPitch) - right.z * sinPitch, right.x * right.z * (1 - cosPitch) + right.y * sinPitch,
            right.y * right.x * (1 - cosPitch) + right.z * sinPitch, cosPitch + right.y * right.y * (1 - cosPitch), right.y * right.z * (1 - cosPitch) - right.x * sinPitch,
            right.z * right.x * (1 - cosPitch) - right.y * sinPitch, right.z * right.y * (1 - cosPitch) + right.x * sinPitch, cosPitch + right.z * right.z * (1 - cosPitch)
        );

        // Rotate the look and up vectors using the pitch rotation
        look = pitchRotation * look;
        up = glm::normalize(glm::cross(right, look)); // Ensure orthogonality

        // Update the camera data
        data.cameraData.look = glm::normalize(glm::vec4(look, 0.0f));
        data.cameraData.up = glm::normalize(glm::vec4(up, 0.0f));
        Camera new_camera = Camera(data.cameraData);

        int h = height();
        int w = width();

        m_proj = new_camera.getProjMatrix(h, w);
        m_view = new_camera.getViewMatrix();

        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around
    float speed = 5.0f;
    glm::vec3 look = glm::normalize(glm::vec3(data.cameraData.look));
    glm::vec3 up = glm::normalize(glm::vec3(data.cameraData.up));
    glm::vec3 right = glm::normalize(glm::vec3(glm::cross(look, up)));

    glm::vec3 movement(0.0f);

    if (m_keyMap[Qt::Key_W]) {
        movement += look * speed * deltaTime;
    }
    if (m_keyMap[Qt::Key_S]) {
        movement -= look * speed * deltaTime;
    }
    if (m_keyMap[Qt::Key_A]) {
        movement -= right * speed * deltaTime;
    }
    if (m_keyMap[Qt::Key_D]) {
        movement += right * speed * deltaTime;
    }
    if (m_keyMap[Qt::Key_Space]) {
        movement += up * speed * deltaTime;
    }
    if (m_keyMap[Qt::Key_Control]) {
        movement -= up * speed * deltaTime;
    }

    data.cameraData.pos[0] += movement[0];
    data.cameraData.pos[1] += movement[1];
    data.cameraData.pos[2] += movement[2];

    Camera new_camera = Camera(data.cameraData);

    int h = height();
    int w = width();

    m_proj = new_camera.getProjMatrix(h, w);
    m_view = new_camera.getViewMatrix();

    update(); // asks for a PaintGL() call to occur
}

// DO NOT EDIT
void Realtime::saveViewportImage(std::string filePath) {
    // Make sure we have the right context and everything has been drawn
    makeCurrent();

    int fixedWidth = 603;
    int fixedHeight = 452;

    // Create Frame Buffer
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Create a color attachment texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fixedWidth, fixedHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // Optional: Create a depth buffer if your rendering uses depth testing
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fixedWidth, fixedHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }

    // Render to the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, fixedWidth, fixedHeight);

    // Clear and render your scene here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paintGL();

    // Read pixels from framebuffer
    std::vector<unsigned char> pixels(fixedWidth * fixedHeight * 3);
    glReadPixels(0, 0, fixedWidth, fixedHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // Unbind the framebuffer to return to default rendering to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Convert to QImage
    QImage image(pixels.data(), fixedWidth, fixedHeight, QImage::Format_RGB888);
    QImage flippedImage = image.mirrored(); // Flip the image vertically

    // Save to file using Qt
    QString qFilePath = QString::fromStdString(filePath);
    if (!flippedImage.save(qFilePath)) {
        std::cerr << "Failed to save image to " << filePath << std::endl;
    }

    // Clean up
    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &fbo);
}
