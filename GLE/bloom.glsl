#version 450

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

uniform vec2 in_res;
uniform sampler2D mip_in;
uniform layout(location=2, rgba16f) image2D mip_out;


// Samples 4 pixels around a point and takes the average.
vec3 quad_sample(sampler2D s, vec2 point, vec2 one_px) {
    return 0.25 * texture(s, point )
}


void main() {
    vec2 one_px = 1.0 / in_res;
    vec2 pixel_coord = vec2(gl_WorkGroupID.xy);
    //vec4 color = imageLoad(mip_in, pixel_coord * 2);
    imageStore(mip_out, pixel_coord, quad_sample(mip_in, pixel_coord, one_px));
}

