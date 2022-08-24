#version 440

layout(location = 0) in vec4 vertex_position;
layout(location = 1) in vec3 normal;

out vec4 vertex_position_worldspace;
out vec3 vertex_normal;

uniform float scale = 1.0;
uniform vec3 offset = vec3(0, 0, 0);
uniform mat4 mvp; // uniforms beginning with _ are hidden in GUI

void main() {
	vertex_position_worldspace = vertex_position; // in the future, we need the model matrix here
	gl_Position = mvp * vec4((vertex_position.xyz * scale) + offset, vertex_position.w);
	vertex_normal = normal;
 }