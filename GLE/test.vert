#version 440

layout(location = 0) in vec4 vertex_position;
uniform float scale = 1.0;
uniform vec3 offset = vec3(0, 0, 0);

void main() {
	gl_Position = vec4((vertex_position.xyz * scale) + offset, vertex_position.w);
 }