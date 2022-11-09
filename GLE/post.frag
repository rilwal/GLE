#version 440 

out vec4 fragColor; 

in vec2 o_vertex_position;
uniform sampler2D input_fb;

uniform float gamma = 2.2;
uniform float exposure = 1.0;


// Let's try Narkowicz ACES
vec3 aces(vec3 color) {
    return (color * (2.51 * color + 0.3)) / (color * (2.43*color+0.59)+ 0.14);
}


void main() {
    vec3 original_color = texture(input_fb, o_vertex_position / 2 + .5).rgb;
    vec3 mapped = aces(original_color); //vec3(1.0) - exp(-original_color * exposure);

    //vec3 mapped = original_color / (original_color + vec3(1.0));
    fragColor = vec4(pow(mapped, vec3(1.0/gamma)), 1);
}

