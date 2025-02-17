#version 330 core
#define MAX_LIGHTS 8

in vec3 fragWorldPosition;
in vec3 fragWorldNormal;

out vec4 fragColor;

uniform int numDirectionalLights;
uniform int numPointLights;
uniform int numSpotLights;

uniform vec3 directionalLightDirections[8];
uniform vec3 directionalLightColors[8];

uniform vec3 pointLightPos[8];
uniform vec3 pointLightColors[8];
uniform vec3 pointFunction[8];

uniform vec3 spotLightDirections[8];
uniform vec3 spotLightPos[8];
uniform vec3 spotLightColors[8];
uniform vec3 spotFunction[8];
uniform float penumbra[8];
uniform float angle[8];

uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;

uniform float k_a;
uniform float k_d;
uniform float k_s;



uniform vec4 viewPosition;
uniform float shininess;

void main() {

    fragColor = vec4(1.0);

    vec3 normal = normalize(fragWorldNormal);

    vec4 ambient_color = vec4(k_a * ambient, 1.0);

    fragColor = ambient_color;

    for (int i = 0; i < numDirectionalLights; ++i) {

        vec3 lightDir = normalize(-directionalLightDirections[i]);
        float diffIntensity = max(dot(normal, lightDir), 0.0);

        vec3 diffuse_color = k_d * diffuse * diffIntensity * directionalLightColors[i];

        vec3 reflectDir = reflect(-lightDir, normal);

        vec3 viewDir = normalize(vec3(viewPosition) - fragWorldPosition);

        float specIntensity = 1.0f;

        if (shininess != 0.0f) {
            specIntensity = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        }
        vec3 specular_color = specIntensity * k_s * specular * directionalLightColors[i];

        fragColor[0] += diffuse_color[0];
        fragColor[1] += diffuse_color[1];
        fragColor[2] += diffuse_color[2];

        fragColor[0] += specular_color[0];
        fragColor[1] += specular_color[1];
        fragColor[2] += specular_color[2];
    }

    for (int i = 0; i < numPointLights; ++i) {

        vec3 lightDir = normalize(fragWorldPosition - pointLightPos[i]);

        float d = length(fragWorldPosition - pointLightPos[i]);

        float attenuation = min(1.0f, 1.0f / (pointFunction[i].x + pointFunction[i].y * d + pointFunction[i].z * d * d));

        float diffIntensity = max(dot(normal, -lightDir), 0.0);

        vec3 diffuse_color = k_d * diffuse * diffIntensity * attenuation * pointLightColors[i];

        vec3 reflectDir = reflect(lightDir, normal);

        vec3 viewDir = normalize(vec3(viewPosition) - fragWorldPosition);

        float specIntensity = 1.0f;

        if (shininess != 0.0f) {
            specIntensity = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        }
        vec3 specular_color = specIntensity * k_s * specular * attenuation * pointLightColors[i];

        fragColor[0] += diffuse_color[0];
        fragColor[1] += diffuse_color[1];
        fragColor[2] += diffuse_color[2];

        fragColor[0] += specular_color[0];
        fragColor[1] += specular_color[1];
        fragColor[2] += specular_color[2];
    }

    for (int i = 0; i < numSpotLights; ++i) {

        vec3 lightDir = normalize(fragWorldPosition - spotLightPos[i]);

        float d = length(fragWorldPosition - spotLightPos[i]);

        float attenuation = min(1.0f, 1.0f / (spotFunction[i].x + spotFunction[i].y * d + spotFunction[i].z * d * d));

        float x = acos(dot(lightDir, spotLightDirections[i]));

        float inner = angle[i] - penumbra[i];

        float base = (x - inner)/(angle[i] - inner);

        float falloff = -2.0f * pow(base, 3.0) + 3.0f * pow(base, 2.0);

        float diffIntensity = max(dot(normal, -lightDir), 0.0);

        vec3 reflectDir = reflect(lightDir, normal);

        vec3 viewDir = normalize(vec3(viewPosition) - fragWorldPosition);

        float specIntensity = 1.0f;

        if (shininess != 0.0f) {
            specIntensity = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        }

        vec3 diffuse_color;
        vec3 specular_color;

        if (x <= inner) {
            vec3 diffuse_color = k_d * diffuse * diffIntensity * attenuation * spotLightColors[i];
            vec3 specular_color = specIntensity * k_s * specular * attenuation * spotLightColors[i];

            fragColor[0] += diffuse_color[0];
            fragColor[1] += diffuse_color[1];
            fragColor[2] += diffuse_color[2];

            fragColor[0] += specular_color[0];
            fragColor[1] += specular_color[1];
            fragColor[2] += specular_color[2];
        }
        else if (x > inner && x <= angle[i]) {
            vec3 diffuse_color = k_d * diffuse * diffIntensity * attenuation * spotLightColors[i] * (1.0f - falloff);
            vec3 specular_color = specIntensity * k_s * specular * attenuation * spotLightColors[i] * (1.0f - falloff);

            fragColor[0] += diffuse_color[0];
            fragColor[1] += diffuse_color[1];
            fragColor[2] += diffuse_color[2];

            fragColor[0] += specular_color[0];
            fragColor[1] += specular_color[1];
            fragColor[2] += specular_color[2];
        }
    }
}
