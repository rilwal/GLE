#version 440
out vec4 fragColour; 

uniform float brightness;

void main() {
	fragColour = vec4(0.0, 1.0, 0.0, 1.0) * brightness;
}