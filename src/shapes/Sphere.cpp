#include "Sphere.h"

void Sphere::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = glm::max(param1, 2);
    m_param2 = glm::max(param2, 3);
    setVertexData();
}

void Sphere::makeTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight) {
    // Task 5: Implement the makeTile() function for a Sphere
    // Note: this function is very similar to the makeTile() function for Cube,
    //       but the normals are calculated in a different way!
    // Calculate normals based on the sphere's geometry (normalized position vectors)
    glm::vec3 normalTopLeft = glm::normalize(topLeft);
    glm::vec3 normalTopRight = glm::normalize(topRight);
    glm::vec3 normalBottomLeft = glm::normalize(bottomLeft);
    glm::vec3 normalBottomRight = glm::normalize(bottomRight);

    // Insert vertices and normals into m_vertexData (triangle 1)
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normalTopLeft);
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normalBottomLeft);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normalBottomRight);

    // Insert vertices and normals into m_vertexData (triangle 2)
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normalTopLeft);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normalBottomRight);
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normalTopRight);

}

void Sphere::makeWedge(float currentTheta, float nextTheta) {
    // Task 6: create a single wedge of the sphere using the
    //         makeTile() function you implemented in Task 5
    // Note: think about how param 1 comes into play here!
    float phiStep = glm::radians(180.f / m_param1);

    float radius = 0.5f;

    for (int i = 0; i < m_param1; ++i) {
        float currentPhi = i * phiStep;
        float nextPhi = (i + 1) * phiStep;

        glm::vec3 topLeft = glm::vec3(radius * sin(currentPhi) * sin(currentTheta),
                                      radius * cos(currentPhi),
                                      radius * sin(currentPhi) * cos(currentTheta));

        glm::vec3 topRight = glm::vec3(radius * sin(currentPhi) * sin(nextTheta),
                                       radius * cos(currentPhi),
                                       radius * sin(currentPhi) * cos(nextTheta));

        glm::vec3 bottomLeft = glm::vec3(radius * sin(nextPhi) * sin(currentTheta),
                                         radius * cos(nextPhi),
                                         radius * sin(nextPhi) * cos(currentTheta));

        glm::vec3 bottomRight = glm::vec3(radius * sin(nextPhi) * sin(nextTheta),
                                          radius * cos(nextPhi),
                                          radius * sin(nextPhi) * cos(nextTheta));

        makeTile(topLeft, topRight, bottomLeft, bottomRight);
    }

}

void Sphere::makeSphere() {
    // Task 7: create a full sphere using the makeWedge() function you
    //         implemented in Task 6
    // Note: think about how param 2 comes into play here!
    float thetaStep = glm::radians(360.f / m_param2);
    for (int i = 0; i < m_param2; ++i) {
        float currentTheta = i * thetaStep;
        float nextTheta = (i + 1) * thetaStep;
        makeWedge(currentTheta, nextTheta);
    }
}

void Sphere::setVertexData() {
    // Uncomment these lines to make a wedge for Task 6, then comment them out for Task 7:

    // float thetaStep = glm::radians(360.f / m_param2);
    // // for (int i = 0; i < m_param2; ++i) {
    //     float currentTheta = 0 * thetaStep;
    //     float nextTheta = 1 * thetaStep;
    //     makeWedge(currentTheta, nextTheta);
    // }

    // Uncomment these lines to make sphere for Task 7:

    makeSphere();
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Sphere::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
