#version 330 core

in vec2 v_texCoord;
out vec4 fragColor;

uniform sampler2D u_renderOutput;
uniform int u_sampleCount;

void main() {
  vec3 linear = texture(u_renderOutput, v_texCoord).rgb / max(u_sampleCount, 1);
  fragColor = vec4(pow(linear, vec3(1.0 / 2.0)), 1.0);
}
