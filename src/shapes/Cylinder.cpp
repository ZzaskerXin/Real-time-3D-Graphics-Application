#include "Cylinder.h"
#include "glm/gtc/constants.hpp"

void Cylinder::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = glm::max(param2, 3);
    setVertexData();
}


void Cylinder::setVertexData() {
    // TODO for Project 5: Lights, Camera
    glm::vec3 topCenter(0.0f, 0.5f, 0.0f);
    glm::vec3 bottomCenter(0.0f, -0.5f, 0.0f);

    float radius = 0.5f;
    float angleStep = glm::two_pi<float>() / static_cast<float>(m_param2);
    float heightStep = 1.0f / static_cast<float>(m_param1);

    // Create top and bottom circles
    for (int i = 0; i < m_param2; ++i) {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;

        // Vertices for top circle
        glm::vec3 topVertex1(radius * cos(angle1), 0.5f, radius * sin(angle1));
        glm::vec3 topVertex2(radius * cos(angle2), 0.5f, radius * sin(angle2));

        // Triangles for the top face (fan around the center)
        insertVec3(m_vertexData, topCenter);
        insertVec3(m_vertexData, glm::vec3(0.0f, 1.0f, 0.0f));

        insertVec3(m_vertexData, topVertex2);
        insertVec3(m_vertexData, glm::vec3(0.0f, 1.0f, 0.0f));

        insertVec3(m_vertexData, topVertex1);
        insertVec3(m_vertexData, glm::vec3(0.0f, 1.0f, 0.0f));

        // Vertices for bottom circle
        glm::vec3 bottomVertex1(radius * cos(angle1), -0.5f, radius * sin(angle1));
        glm::vec3 bottomVertex2(radius * cos(angle2), -0.5f, radius * sin(angle2));

        // Triangles for the bottom face (fan around the center)
        insertVec3(m_vertexData, bottomCenter);
        insertVec3(m_vertexData, glm::vec3(0.0f, -1.0f, 0.0f));

        insertVec3(m_vertexData, bottomVertex1);
        insertVec3(m_vertexData, glm::vec3(0.0f, -1.0f, 0.0f));

        insertVec3(m_vertexData, bottomVertex2);
        insertVec3(m_vertexData, glm::vec3(0.0f, -1.0f, 0.0f));
    }

    // Create side faces
    for (int i = 0; i < m_param2; ++i) {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;

        glm::vec3 bottomVertex1(radius * cos(angle1), -0.5f, radius * sin(angle1));
        glm::vec3 bottomVertex2(radius * cos(angle2), -0.5f, radius * sin(angle2));
        glm::vec3 topVertex1(radius * cos(angle1), 0.5f, radius * sin(angle1));
        glm::vec3 topVertex2(radius * cos(angle2), 0.5f, radius * sin(angle2));

        glm::vec3 normal1 = glm::normalize(glm::vec3(cos(angle1), 0.0f, sin(angle1)));
        glm::vec3 normal2 = glm::normalize(glm::vec3(cos(angle2), 0.0f, sin(angle2)));

        for (int j = 0; j < m_param1; ++j) {
            float currentHeight = -0.5f + j * heightStep;
            float nextHeight = currentHeight + heightStep;

            glm::vec3 bottomLeft = glm::vec3(bottomVertex1.x, currentHeight, bottomVertex1.z);
            glm::vec3 bottomRight = glm::vec3(bottomVertex2.x, currentHeight, bottomVertex2.z);
            glm::vec3 topLeft = glm::vec3(bottomVertex1.x, nextHeight, bottomVertex1.z);
            glm::vec3 topRight = glm::vec3(bottomVertex2.x, nextHeight, bottomVertex2.z);

            // First triangle
            insertVec3(m_vertexData, bottomLeft);
            insertVec3(m_vertexData, normal1);

            insertVec3(m_vertexData, topLeft);
            insertVec3(m_vertexData, normal1);

            insertVec3(m_vertexData, bottomRight);
            insertVec3(m_vertexData, normal2);

            // Second triangle
            insertVec3(m_vertexData, bottomRight);
            insertVec3(m_vertexData, normal2);

            insertVec3(m_vertexData, topLeft);
            insertVec3(m_vertexData, normal1);

            insertVec3(m_vertexData, topRight);
            insertVec3(m_vertexData, normal2);
        }
    }
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cylinder::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
