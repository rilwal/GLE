#version 440

layout(location = 0) in vec4 vertex_position;
layout(location = 1) in vec3 normal;

out vec4 vertex_position_worldspace;
out vec3 vertex_normal;

uniform float scale = 1.0;
uniform vec3 offset = vec3(0, 0, 0);

uniform float time_offset = 0.0;

uniform mat4 _mvp; // uniforms beginning with _ are hidden in GUI
uniform mat4 _model; // _mvp and _model get the appropriate matrices automatically
uniform float _time; // get the time

void main() {
	vec4 vp = vertex_position * vec4(1, max((sin(_time + time_offset) / 2 + 1.5) / 2, .5), 1, 1);
	vertex_position_worldspace = _model * vp;
	gl_Position = _mvp * vp;
	vertex_normal = (inverse(transpose(_model)) * vec4(normal, 1)).xyz;
 }