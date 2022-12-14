#version 450

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

uniform vec2 in_res;
uniform layout(location=1, rgba16f) readonly  image2D mip_in;
uniform layout(location=2, rgba16f) writeonly image2D mip_out;


// Because the image is always 2x the size, I think this will suffice for bilinear
// It may lose accuracy at low resoltions due to rounding errors (when the image is of odd dimension)
vec4 bilinear_sample(ivec2 px) {
    return  imageLoad(mip_in, px) * 0.25 +
            imageLoad(mip_in, px + ivec2(0, 1)) * 0.25 +
            imageLoad(mip_in, px + ivec2(1, 0)) * 0.25 +
            imageLoad(mip_in, px + ivec2(1, 1)) * 0.25;    
}


// Samples 4 pixels around a point and takes the average.
vec4 quad_sample(ivec2 px) {
    return bilinear_sample(px) * 0.5 +
        bilinear_sample(px + ivec2(-1, -1)) * 0.125 +
        bilinear_sample(px + ivec2( 1, -1)) * 0.125 +
        bilinear_sample(px + ivec2(-1,  1)) * 0.125 +
        bilinear_sample(px + ivec2( 1,  1)) * 0.125;
}


void main() {
    ivec2 pixel_coord = ivec2(gl_WorkGroupID.xy);
    imageStore(mip_out, pixel_coord, quad_sample(pixel_coord * 2));
}

