#version 330 core

layout(location = 0) in vec3 position3d;
layout(location = 1) in vec3 normal;

out vec3 fragWorldPosition;
out vec3 fragWorldNormal;

uniform mat4 modelMatrix;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() {
    fragWorldPosition = vec3(modelMatrix * vec4(position3d, 1.0));


    mat3 normalMatrix = mat3(inverse(transpose(modelMatrix)));
    fragWorldNormal = normalize(normalMatrix * normal);

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position3d, 1.0f);
}
