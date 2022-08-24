#version 440
out vec4 fragColour; 

uniform float brightness = 1;
uniform vec3 color = vec3(1, 0, 0); 

void main() {
	fragColour = vec4(color.r, color.g, color.b, 1.0) * brightness;
}