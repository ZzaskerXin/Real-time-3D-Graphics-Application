#version 330 core
in vec2 fragUV;

uniform sampler2D t;

uniform bool pixel;
uniform bool kernel;

const float Filterkernel[9] = float[](
    -1.0 / 9.0, -1.0 / 9.0, -1.0 / 9.0,
    -1.0 / 9.0,  17.0 / 9.0, -1.0 / 9.0,
    -1.0 / 9.0, -1.0 / 9.0, -1.0 / 9.0
);

out vec4 fragColor;

void main()
{
    // vec3 color = texture(t, fragUV).rgb;

    // fragColor = vec4(1);

    // fragColor = texture(t, fragUV);

    // if (pixel) {
    //     fragColor.rgb = vec3(1.0) - fragColor.rgb;
    // }

    // if (kernel) {
    //     ivec2 texSize = textureSize(t, 0);
    //     vec2 texOffset = 1.0 / vec2(texSize);

    //     vec3 result = vec3(0.0);

    //     int index = 0;
    //     for (int y = -1; y <= 1; y++) {
    //         for (int x = -1; x <= 1; x++) {
    //             vec2 offset = vec2(x, y) * texOffset;
    //             vec3 texColor = texture(t, fragUV + offset).rgb;
    //             result += texColor * Filterkernel[index];
    //             index++;
    //         }
    //     }
    //     fragColor = vec4(result, 1.0);
    // }
    vec3 baseColor = texture(t, fragUV).rgb;
    vec3 resultColor = baseColor;


    if (kernel) {
        ivec2 texSize = textureSize(t, 0);
        vec2 texOffset = 1.0 / vec2(texSize);

        resultColor = vec3(0.0);
        int index = 0;

        for (int y = -1; y <= 1; y++) {
            for (int x = -1; x <= 1; x++) {
                vec2 offset = vec2(x, y) * texOffset;
                vec3 texColor = texture(t, fragUV + offset).rgb;
                resultColor += texColor * Filterkernel[index];
                index++;
            }
        }
    }

    if (pixel) {
        resultColor = vec3(1.0) - resultColor;
    }

    fragColor = vec4(resultColor, 1.0);
}
