#version 440

layout(location = 0) in vec4 vertex_position;
layout(location = 1) in vec3 normal;

out vec4 vertex_position_worldspace;
out vec3 vertex_normal;

uniform float scale = 1.0;
uniform vec3 offset = vec3(0, 0, 0);

uniform mat4 _mvp; // uniforms beginning with _ are hidden in GUI
uniform mat4 _model; // _mvp and _model get the appropriate matrices automatically

void main() {
	vertex_position_worldspace = _model * vertex_position;
	gl_Position = _mvp * vertex_position;
	vertex_normal = (inverse(transpose(_model)) * vec4(normal, 1)).xyz;
 }