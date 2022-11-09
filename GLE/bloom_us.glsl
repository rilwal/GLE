#version 450

// This shader upscales for bloom effect


layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

uniform layout(location=0, rgba16f) readonly image2D mip_in;
uniform layout(location=1, rgba16f) image2D mip_out;
uniform layout(location=2) float radius = 1.0;
uniform layout(location=3) float intensity = 1.0;

vec4 lerp(vec4 a, vec4 b, float d) {
    return (1.0 - d) * a + d * b;
}


// For upscaling we need a real bilinear
// pt is in image space (0,0) -> (width, height) 
vec4 bilinear_sample(vec2 pt) {
    ivec2 px = ivec2(pt); // truncated pixel as integer
    vec2 lerp_factor = pt - vec2(px); // subtract the whole part to get the lerp factors
    vec4 horizontal_sample_1 = lerp(imageLoad(mip_in, px + ivec2(0, 0)), imageLoad(mip_in, px + ivec2(1, 0)), lerp_factor.x);
    vec4 horizontal_sample_2 = lerp(imageLoad(mip_in, px + ivec2(1, 0)), imageLoad(mip_in, px + ivec2(1, 1)), lerp_factor.x);

    //return lerp(horizontal_sample_1, horizontal_sample_2, lerp_factor.y);
    return imageLoad(mip_in, px + ivec2(0, 0));
}


vec4 tent_upsample(vec2 px) {
    float r = radius;
    return (
        bilinear_sample(px + vec2(-r, -r)) * 1 +
        bilinear_sample(px + vec2( 0, -r)) * 2 +
        bilinear_sample(px + vec2( r, -r)) * 1 +
        bilinear_sample(px + vec2(-r,  0)) * 2 +
        bilinear_sample(px + vec2( 0,  0)) * 4 +
        bilinear_sample(px + vec2( r,  0)) * 2 +
        bilinear_sample(px + vec2(-r,  r)) * 1 +
        bilinear_sample(px + vec2( 0,  r)) * 2 +
        bilinear_sample(px + vec2( r,  r)) * 1) * 1/16;
}


void main() {
    ivec2 pixel_coord = ivec2(gl_WorkGroupID.xy);
    vec4 original = imageLoad(mip_out, pixel_coord);
    imageStore(mip_out, pixel_coord, original + intensity * tent_upsample(vec2(pixel_coord) / 2));
}

