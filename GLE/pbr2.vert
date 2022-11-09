#version 440

layout(location = 0) in vec4 vertex_position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

out vec4 vertex_position_worldspace;
out vec3 vertex_normal;
out vec2 v_uv;


uniform float scale = 1.0;



uniform mat4 _mvp; // uniforms beginning with _ are hidden in GUI
uniform mat4 _model; // _mvp and _model get the appropriate matrices automatically
uniform float _time; // get the time


void main() {
	vec4 vp = vertex_position;
	vertex_position_worldspace = _model * vp;
	gl_Position = _mvp * vp;


    v_uv = vec2(uv.x, uv.y);
	vertex_normal = normalize((inverse(transpose(_model)) * vec4(normal, 1)).xyz);
 }

 