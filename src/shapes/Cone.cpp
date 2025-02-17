#include "Cone.h"
#include "glm/gtc/constants.hpp"

void Cone::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = glm::max(param2, 3);
    setVertexData();
}


void Cone::setVertexData() {
    // TODO for Project 5: Lights, Camera
    const float angleStep = 2 * M_PI / m_param2;
    const float radiusStep = m_radius / m_param1;
    const glm::vec3 center(0.0f, -0.5f, 0.0f);
    const glm::vec3 normal(0.0f, -1.0f, 0.0f);

    for (int i = 0; i < m_param1; ++i) {
        float radius1 = i * radiusStep;
        float radius2 = (i + 1) * radiusStep;

        for (int j = 0; j < m_param2; ++j) {
            float theta = j * angleStep;
            float nextTheta = (j + 1) * angleStep;

            glm::vec3 innerLeft(radius1 * cos(theta), -0.5f, radius1 * sin(theta));
            glm::vec3 innerRight(radius1 * cos(nextTheta), -0.5f, radius1 * sin(nextTheta));
            glm::vec3 outerLeft(radius2 * cos(theta), -0.5f, radius2 * sin(theta));
            glm::vec3 outerRight(radius2 * cos(nextTheta), -0.5f, radius2 * sin(nextTheta));

            insertVec3(m_vertexData, innerLeft);
            insertVec3(m_vertexData, normal);

            insertVec3(m_vertexData, outerLeft);
            insertVec3(m_vertexData, normal);

            insertVec3(m_vertexData, outerRight);
            insertVec3(m_vertexData, normal);

            insertVec3(m_vertexData, innerLeft);
            insertVec3(m_vertexData, normal);

            insertVec3(m_vertexData, outerRight);
            insertVec3(m_vertexData, normal);

            insertVec3(m_vertexData, innerRight);
            insertVec3(m_vertexData, normal);
        }
    }

    const float invertedAngleStep = -angleStep;
    const float heightStep = 1.0f / m_param1;
    const glm::vec3 tip(0.0f, 0.5f, 0.0f);

    for (int i = 0; i < m_param2; ++i) {
        float theta = i * invertedAngleStep;
        float nextTheta = (i + 1) * invertedAngleStep;

        for (int j = 0; j < m_param1; ++j) {
            float currentheight = -0.5f + j * heightStep;
            float nextheight = -0.5f + (j + 1) * heightStep;
            float radius1 = 0.5f * (1 - static_cast<float>(j) / m_param1);
            float radius2 = 0.5f * (1 - static_cast<float>(j + 1) / m_param1);

            glm::vec3 bottomLeft(radius1 * cos(theta), currentheight, radius1 * sin(theta));
            glm::vec3 bottomRight(radius1 * cos(nextTheta), currentheight, radius1 * sin(nextTheta));
            glm::vec3 topLeft(radius2 * cos(theta), nextheight, radius2 * sin(theta));
            glm::vec3 topRight(radius2 * cos(nextTheta), nextheight, radius2 * sin(nextTheta));

            glm::vec3 normalBottomLeft = glm::normalize(glm::vec3(2 * bottomLeft.x, 0.25f - 0.5f * bottomLeft.y, 2 * bottomLeft.z));
            glm::vec3 normalBottomRight = glm::normalize(glm::vec3(2 * bottomRight.x, 0.25f - 0.5f * bottomRight.y, 2 * bottomRight.z));
            glm::vec3 normalTopLeft = glm::normalize(glm::vec3(2 * topLeft.x, 0.25f - 0.5f * topLeft.y, 2 * topLeft.z));
            glm::vec3 normalTopRight = glm::normalize(glm::vec3(2 * topRight.x, 0.25f - 0.5f * topRight.y, 2 * topRight.z));

            if (j == m_param1 - 1) {
                normalTopLeft = glm::normalize(normalBottomLeft + normalBottomRight);
                normalTopRight = glm::normalize(normalBottomLeft + normalBottomRight);
            }

            insertVec3(m_vertexData, bottomLeft);
            insertVec3(m_vertexData, normalBottomLeft);

            insertVec3(m_vertexData, bottomRight);
            insertVec3(m_vertexData, normalBottomRight);

            insertVec3(m_vertexData, topLeft);
            insertVec3(m_vertexData, normalTopLeft);

            insertVec3(m_vertexData, topRight);
            insertVec3(m_vertexData, normalTopRight);

            insertVec3(m_vertexData, topLeft);
            insertVec3(m_vertexData, normalTopLeft);

            insertVec3(m_vertexData, bottomRight);
            insertVec3(m_vertexData, normalBottomRight);
        }
    }
    // glm::vec3 tip(0.0f, 0.5f, 0.0f);
    // glm::vec3 bottomCenter(0.0f, -0.5f, 0.0f);

    // float radius = 0.5f;
    // float angleStep = glm::two_pi<float>() / static_cast<float>(m_param2);
    // float heightStep = 1.0f / static_cast<float>(m_param1);

    // // Create bottom circle
    // for (int i = 0; i < m_param2; ++i) {
    //     float angle1 = i * angleStep;
    //     float angle2 = (i + 1) * angleStep;

    //     // Vertices for bottom circle
    //     glm::vec3 bottomVertex1(radius * cos(angle1), -0.5f, radius * sin(angle1));
    //     glm::vec3 bottomVertex2(radius * cos(angle2), -0.5f, radius * sin(angle2));

    //     // Triangles for the bottom face (fan around the center)
    //     insertVec3(m_vertexData, bottomCenter);
    //     insertVec3(m_vertexData, glm::vec3(0.0f, -1.0f, 0.0f));

    //     insertVec3(m_vertexData, bottomVertex1);
    //     insertVec3(m_vertexData, glm::vec3(0.0f, -1.0f, 0.0f));

    //     insertVec3(m_vertexData, bottomVertex2);
    //     insertVec3(m_vertexData, glm::vec3(0.0f, -1.0f, 0.0f));
    // }

    // // Create side faces
    // for (int i = 0; i < m_param2; ++i) {
    //     float angle1 = i * angleStep;
    //     float angle2 = (i + 1) * angleStep;

    //     glm::vec3 bottomVertex1(radius * cos(angle1), -0.5f, radius * sin(angle1));
    //     glm::vec3 bottomVertex2(radius * cos(angle2), -0.5f, radius * sin(angle2));

    //     glm::vec3 normal1 = glm::normalize(glm::vec3(cos(angle1), 0.5f, sin(angle1)));
    //     glm::vec3 normal2 = glm::normalize(glm::vec3(cos(angle2), 0.5f, sin(angle2)));

    //     for (int j = 0; j < m_param1; ++j) {
    //         float currentHeight = -0.5f + j * heightStep;
    //         float nextHeight = currentHeight + heightStep;

    //         glm::vec3 bottomLeft = glm::vec3(bottomVertex1.x, currentHeight, bottomVertex1.z);
    //         glm::vec3 bottomRight = glm::vec3(bottomVertex2.x, currentHeight, bottomVertex2.z);
    //         glm::vec3 topLeft = glm::vec3(bottomVertex1.x, nextHeight, bottomVertex1.z);
    //         glm::vec3 topRight = glm::vec3(bottomVertex2.x, nextHeight, bottomVertex2.z);

    //         insertVec3(m_vertexData, bottomVertex2);
    //         insertVec3(m_vertexData, normal2);

    //         insertVec3(m_vertexData, bottomVertex1);
    //         insertVec3(m_vertexData, normal1);

    //         insertVec3(m_vertexData, tip);
    //         insertVec3(m_vertexData, glm::normalize(tip - bottomVertex1));
    //     }
    // }
}


// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cone::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
