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
uniform vec3 light_pos = vec3(20, 20, 20);


// Material properties. For now implemented as uniform, but these should come from a map later
uniform vec3 c_albedo; // The albedo coloring
uniform float metalness; // The metalness

uniform sampler2D emissive_map;
uniform sampler2D albedo_map;
uniform sampler2D normal_map;
uniform sampler2D metalness_map;
uniform sampler2D roughness_map;
uniform sampler2D ao_map;

uniform samplerCube cubemap;

//uniform mat4 _model;

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

vec3 fresnel_factor(vec3 f0, float product)
{
    return mix(f0, vec3(1.0), pow(1.01 - product, 5.0));
}


// simple phong specular calculation with normalization
vec3 phong_specular(vec3 V, vec3 L, vec3 N, vec3 specular, float roughness)
{
    vec3 R = reflect(-L, N);
    float spec = max(0.0, dot(V, R));

    float k = 1.999 / (roughness * roughness);

    return min(1.0, 3.0 * 0.0398 * k) * pow(spec, min(10000.0, k)) * specular;
}

// phong (lambertian) diffuse term
float phong_diffuse()
{
    return (1.0 / 3.1415926);
}



void main() {
	
	vec3 emission = texture(emissive_map, v_uv).xyz;


	float A = 10.0 / dot(light_pos - vertex_position_worldspace.xyz, light_pos - vertex_position_worldspace.xyz);
	
	vec3 L = normalize(light_pos - vertex_position_worldspace.xyz);
	vec3 V = normalize(-gl_FragCoord.xyz);
	vec3 H = normalize(L + V);
	vec3 nn = normalize(vertex_normal);

	mat3 tbn = cotangent_frame(vertex_normal, vertex_position_worldspace.xyz, v_uv);

	vec3 N = normalize(tbn * (texture(normal_map, v_uv).xyz * 2 - 1));

	vec3 albedo = texture(albedo_map, v_uv).xyz;

	float roughness = texture(roughness_map, v_uv).y;

    float NdL = max(0.0, dot(N, L));
    float NdV = max(0.001, dot(N, V));
    float NdH = max(0.001, dot(N, H));
    float HdV = max(0.001, dot(H, V));
    float LdV = max(0.001, dot(L, V));

	vec3 specular = vec3(1.0);

	//vec3 env = texture(cubemap, N).xyz;

	vec3 refl = reflect(-V, N);
	vec3 envspec = texture(cubemap, refl).xyz;

	vec3 specfresnel = fresnel_factor(specular, NdV);
	vec3 specref = phong_specular(V, L, N, specfresnel, roughness);

	specref *= vec3(NdL);

	vec3 diffref = (vec3(1) * albedo) * phong_diffuse() * NdL;

	vec3 reflected_light = vec3(0);
	vec3 diffused_light = vec3(0);

	vec3 light_color = c_lightcolor;
	reflected_light += specref * light_color;
	diffused_light += diffref * light_color;

	//vec3 iblspec = min(vec3(0.99), fresnel_factor(specular, NdV) * brdf.x + brdf.y);


	//reflected_light += texture(cubemap, N).xyz;

	//fragColour = texture(cubemap, N);

	fragColour = vec4(emission + diffused_light + reflected_light, 1.0);
}

