#version 440 

layout(location = 0) in vec2 vertex_position;

out vec2 o_vertex_position;


void main() {
    o_vertex_position = vertex_position;
    gl_Position = vec4(vertex_position, 0, 1);
}

