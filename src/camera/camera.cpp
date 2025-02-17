#include "camera.h"
#include "settings.h"

Camera::Camera(const SceneCameraData &data) : cameraData(data) {}


glm::mat4 Camera::getViewMatrix() const {
    // Optional TODO: implement the getter or make your own design
    // glm::vec3 eye = glm::vec3(cameraData.pos);
    // glm::vec3 center = glm::vec3(cameraData.look);
    // glm::vec3 up = glm::vec3(cameraData.up);

    // return glm::lookAt(eye, center, up);

    glm::vec3 w = glm::normalize(glm::vec3(-cameraData.look));

    glm::vec3 v = glm::normalize(glm::vec3(cameraData.up) - (glm::dot(glm::vec3(cameraData.up), w) * w));

    glm::vec3 u = glm::cross(v, w);

    glm::mat4 R = glm::mat4(u[0], v[0], w[0], 0,
                            u[1], v[1], w[1], 0,
                            u[2], v[2], w[2], 0,
                            0, 0, 0, 1);
    glm::mat4 T = glm::mat4(1, 0, 0, 0,
                            0, 1, 0, 0,
                            0, 0, 1, 0,
                            cameraData.pos[0] * (-1), cameraData.pos[1] * (-1), cameraData.pos[2] * (-1), 1);

    return R * T;
}

glm::mat4 Camera::getInverseViewMatrix() const {
    return glm::inverse(getViewMatrix());
}

glm::mat4 Camera::getProjMatrix(int height, int width) const {
//     float c = -settings.nearPlane/settings.farPlane;

//     float angle_h = cameraData.heightAngle;

//     float angle_w = cameraData.

//     return glm::mat4(1, 0, 0, 0,
//                      0, 1, 0, 0,
//                      0, 0, 1/(1 + c), -1,
//                      0, 0, -c/(1 + c), 0);


    float aspectR = float(width) / float(height);
    float tanHalfFov = tan(cameraData.heightAngle / 2.0f);

    glm::mat4 m_proj = glm::mat4(0.0f);
    m_proj[0][0] = 1.0f / (aspectR * tanHalfFov);
    m_proj[1][1] = 1.0f / tanHalfFov;
    m_proj[2][2] = -(settings.farPlane + settings.nearPlane) / (settings.farPlane - settings.nearPlane);
    m_proj[2][3] = -1.0f;
    m_proj[3][2] = -(2.0f * settings.farPlane * settings.nearPlane) / (settings.farPlane - settings.nearPlane);
    return m_proj;

}
// float Camera::getAspectRatio() const {
//     // Optional TODO: implement the getter or make your own design
//     return cameraData.;
// }

float Camera::getHeightAngle() const {
    // Optional TODO: implement the getter or make your own design
    return cameraData.heightAngle;
}

SceneCameraData Camera::getcameraData() const {
    return cameraData;
}

// float Camera::getFocalLength() const {
//     // Optional TODO: implement the getter or make your own design

// }

// float Camera::getAperture() const {
//     // Optional TODO: implement the getter or make your own design

// }

