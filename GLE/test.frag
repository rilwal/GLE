#version 440
out vec4 fragColour; 

in vec4 vertex_position_worldspace;
in vec3 vertex_normal;

uniform float brightness = 1;
uniform vec3 c_color = vec3(1, 0, 0); 
uniform vec3 c_lightcolor = vec3(1, 1, 1);
uniform vec3 light_pos = vec3(20, 20, 20);


void main() {
	float ambient = 0.1;
	vec3 lightDir = normalize(light_pos - vertex_position_worldspace.xyz);
	float diff = dot(vertex_normal, lightDir);
	vec3 diffuse = diff * c_lightcolor;

	fragColour = vec4(vec3(diff), 1); //vec4(c_color * diffuse + c_color * ambient, 1.0)  * brightness;
}