#version 330

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

uniform mat4 mvp;
uniform mat4 matNormal;

uniform vec3 directionToLight;
uniform vec3 sunColor;
uniform vec3 ambientColor;

out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 fragLightFront;
out vec3 fragLightBack;

void main() {
    vec3 normal = normalize(
        (matNormal * vec4(vertexNormal, 0.0)).xyz
    );

    float facing = dot(normal, directionToLight);

    fragLightFront = ambientColor + sunColor * max(facing, 0.0);
    fragLightBack = ambientColor + sunColor * max(-facing, 0.0);

    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;

    gl_Position = mvp * vec4(vertexPosition, 1.0);
}