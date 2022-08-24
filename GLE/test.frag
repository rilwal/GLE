#version 440
out vec4 fragColour; 

uniform float brightness;
uniform vec3 color; 

void main() {
	fragColour = vec4(color.r, color.g, color.b, 1.0) * brightness;
}