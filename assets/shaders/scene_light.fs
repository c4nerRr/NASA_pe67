#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragLightFront;
in vec3 fragLightBack;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

out vec4 finalColor;

void main() {
    vec4 baseColor =
        texture(texture0, fragTexCoord) * colDiffuse * fragColor;

    vec3 lighting = gl_FrontFacing  ? fragLightFront : fragLightBack;

    finalColor = vec4( baseColor.rgb * lighting, baseColor.a);
}