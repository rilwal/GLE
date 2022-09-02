#version 440
out vec4 fragColour; 

in vec4 vertex_position_worldspace;
in vec3 vertex_normal;


uniform float brightness = 1;
uniform float ambient_light = 0.2;
uniform vec3 c_color = vec3(1, 0, 0); 
uniform vec3 c_lightcolor = vec3(1, 1, 1);
uniform vec3 light_pos = vec3(20, 20, 20);

//uniform mat4 _model;

void main() {
	vec3 lightDir = normalize((light_pos) - vertex_position_worldspace.xyz);
	float diff = max(0, dot(vertex_normal, lightDir));
	vec3 diffuse = diff * c_lightcolor;

	fragColour = vec4(c_color * diffuse + c_color * ambient_light, 1.0)  * brightness;
}