#version 440
out vec4 fragColour; 

uniform float brightness = 1;
uniform vec3 c_color = vec3(1, 0, 0); 

void main() {
	fragColour = vec4(c_color.r, c_color.g, c_color.b, 1.0) * brightness;
}