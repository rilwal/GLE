#version 440

layout(location = 0) in vec4 vertex_position;
uniform float scale = 1.0;

void main() {
	gl_Position = vec4(vertex_position.xy * scale, vertex_position.zw);
 }