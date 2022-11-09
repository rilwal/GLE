#version 450

// This shader does downsampling for bloom effect


layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

uniform layout(location=1, rgba16f) readonly  image2D mip_in;
uniform layout(location=2, rgba16f) writeonly image2D mip_out;

vec4 lerp(vec4 a, vec4 b, float d) {
    return (1.0 - d) * a + d * b;
}


// Because the image is always 2x the size, I think this will suffice for bilinear
// It may lose accuracy at low resoltions due to rounding errors (when the image is of odd dimension)
vec4 bilinear_sample(ivec2 px) {
    return  imageLoad(mip_in, px) * 0.25 +
            imageLoad(mip_in, px + ivec2(0, 1)) * 0.25 +
            imageLoad(mip_in, px + ivec2(1, 0)) * 0.25 +
            imageLoad(mip_in, px + ivec2(1, 1)) * 0.25;    
}

/*
// For upscaling we need a real bilinear
// pt is in image space (0,0) -> (width, height) 
vec4 bilinear_sample(vec2 pt) {
    ivec2 px = ivec2(pt); // truncated pixel as integer
    vec2 lerp_factor = pt - vec2(px); // subtract the whole part to get the lerp factors
    vec4 horizontal_sample_1 = lerp(imageLoad(mip_in, px + ivec2(0, 0)), imageLoad(mip_in, px + ivec2(1, 0)), lerp_factor.x);
    vec4 horizontal_sample_2 = lerp(imageLoad(mip_in, px + ivec2(1, 0)), imageLoad(mip_in, px + ivec2(1, 1)), lerp_factor.x);

    return lerp(horizontal_sample_1, horizontal_sample_2, lerp_factor.y);
}
*/

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

