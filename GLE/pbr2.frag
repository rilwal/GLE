#version 440 

out vec4 fragColour; 

in vec4 vertex_position_worldspace;
in vec3 vertex_normal;
in vec2 v_uv;

uniform mat4 _model; 

uniform float brightness = 1;
uniform float ambient_light = 0.2;
uniform vec3 c_color = vec3(1, 1, 1); 
uniform vec3 c_lightcolor = vec3(1, 1, 1);
uniform vec3 light_pos = vec3(3, 3, 5);


// Material properties. For now implemented as uniform, but these should come from a map later
uniform vec3 c_albedo = vec3(1,1,1); // The albedo coloring
uniform float metallic;
uniform float roughness;
uniform float ao;

uniform sampler2D emissive_map;
uniform sampler2D albedo_map;
uniform sampler2D normal_map;
uniform sampler2D metalness_map;
uniform sampler2D roughness_map;
uniform sampler2D ao_map;

uniform samplerCube env;


const float PI = 3.1415;

mat3 cotangent_frame( vec3 N, vec3 p, vec2 uv )
{
    // get edge vectors of the pixel triangle
    vec3 dp1 = dFdx( p );
    vec3 dp2 = dFdy( p );
    vec2 duv1 = dFdx( uv );
    vec2 duv2 = dFdy( uv );

    // solve the linear system
    vec3 dp2perp = cross( dp2, N );
    vec3 dp1perp = cross( N, dp1 );
    vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;

    // construct a scale-invariant frame 
    float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );
    return mat3( T * invmax, B * invmax, N );
}


vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}  

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}


void main() {
    vec3 albedo = texture(albedo_map, v_uv).xyz * c_albedo;
   	mat3 tbn = cotangent_frame(vertex_normal, vertex_position_worldspace.xyz, v_uv);
    vec3 N = normalize(tbn * (texture(normal_map, v_uv).xyz * 2 - 1));

    vec3 V = normalize(-gl_FragCoord.xyz);



    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);
    //for (int i = 0; i < 1; i++) {
        vec3 L = normalize(light_pos - vertex_position_worldspace.xyz);
        vec3 H = normalize(V + L);
        float distance_2 = dot(light_pos - vertex_position_worldspace.xyz, light_pos - vertex_position_worldspace.xyz);
        float attenuation = 1.0 / distance_2;
        vec3 radiance = c_lightcolor * attenuation;

        float roughness = texture(roughness_map, v_uv).x;

        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD + albedo / PI + specular) * radiance * NdotL;
    //}

    vec3 ambient = vec3(0.1) * albedo * texture(ao_map, v_uv).xyz;
    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));

    vec3 emission = texture(emissive_map, v_uv).xyz;

    fragColour = vec4(emission + color, 1.0);
}

// This is the end of the shader