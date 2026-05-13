#version 330 core

in vec2 v_texCoord;
out vec4 fragColor;

uniform sampler2D u_renderOutput;

void main() {
  vec3 linear = texture(u_renderOutput, v_texCoord).rgb;
  fragColor = vec4(pow(linear, vec3(1.0 / 2.0)), 1.0);
}
