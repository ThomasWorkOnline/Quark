#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Normal;

layout(std140, binding = 0) uniform Camera {
	mat4 u_ViewProjection;
	vec4 u_CameraPosition;
};

layout(location = 0) out VertexOutput {
	vec3 Position;
	vec2 TexCoord;
	vec3 Normal;
} Output;

void main()
{
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
	
	Output.Position = a_Position;
	Output.TexCoord = a_TexCoord;
	Output.Normal   = a_Normal;
}

#type fragment
#version 450 core

const float PI = 3.14159265359;

vec3  GetNormalFromMap();
float DistributionGGX(vec3 n, vec3 h, float roughness);
float GeometrySchlickGGX(float nDotV, float k);
float GeometrySmith(vec3 n, vec3 v, vec3 l, float k);
vec3  FresnelSchlick(float cosTheta, vec3 f0);

layout(std140, binding = 0) uniform Camera {
	mat4 u_ViewProjection;
	vec4 u_CameraPosition;
};

layout(location = 0) in VertexOutput {
	vec3 Position;
	vec2 TexCoord;
	vec3 Normal;
} Input;

struct Material
{
	sampler2D AlbedoMap;
	sampler2D NormalMap;
	sampler2D MetallicMap;
	sampler2D RoughnessMap;
	sampler2D AmbiantOcclusionMap;
};

const uint s_LightCount = 4;
uniform vec3 u_LightPositions[s_LightCount];
uniform vec3 u_LightColors[s_LightCount];

uniform samplerCube u_IrradianceMap;
uniform Material u_Material;

layout(location = 0) out vec4 o_Color;

// Shader source from learnopengl:
// https://learnopengl.com/code_viewer_gh.php?code=src/6.pbr/1.2.lighting_textured/1.2.pbr.fs
void main()
{
	vec3  albedo    = texture(u_Material.AlbedoMap,           Input.TexCoord).rgb;
	float metallic  = texture(u_Material.MetallicMap,         Input.TexCoord).r;
	float roughness = texture(u_Material.RoughnessMap,        Input.TexCoord).r;
	float ao        = texture(u_Material.AmbiantOcclusionMap, Input.TexCoord).r;
	
	vec3 N = GetNormalFromMap();
	vec3 V = normalize(u_CameraPosition.xyz - Input.Position);
	vec3 R = reflect(-V, N);
	
	// Calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
	// of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);
	
	// Reflectance equation
	vec3 Lo = vec3(0.0);
	for (uint i = 0; i < s_LightCount; ++i)
	{
		// Calculate per-light radiance
		vec3 L            = normalize(u_LightPositions[i] - Input.Position);
		vec3 H            = normalize(V + L);
		float distance    = length(u_LightPositions[i] - Input.Position);
		float attenuation = 1.0 / (distance * distance);
		vec3  radiance    = u_LightColors[i] * attenuation;
		
		// Cook-Torrance BRDF
		float NDF         = DistributionGGX(N, H, roughness);
		float G           = GeometrySmith(N, V, L, roughness);
		vec3 F            = FresnelSchlick(max(dot(H, V), 0.0), F0);
		
		vec3 numerator    = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0001);
		vec3 specular     = numerator / denominator;
		
		// kS is equal to Fresnel
		vec3 kS           = F;
		// For energy conservation, the diffuse and specular light can't
		// be above 1.0 (unless the surface emits light); to preserve this
		// relationship the diffuse component (kD) should equal 1.0 - kS.
		vec3 kD           = vec3(1.0) - kS;
		// Multiply kD by the inverse metalness such that only non-metals
		// have diffuse lighting, or a linear blend if partly metal (pure metals
		// have no diffuse light).
		kD               *= 1.0 - metallic;
		    
		// Scale light by NdotL
		float NdotL       = max(dot(N, L), 0.0);
		
		// Add to outgoing radiance Lo
		Lo               += (kD * albedo / PI + specular) * radiance * NdotL; // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
	}   
	
	// Ambient lighting (we now use IBL as the ambient term)
	vec3 kS               = FresnelSchlick(max(dot(N, V), 0.0), F0);
	vec3 kD               = 1.0 - kS;
	kD                   *= 1.0 - metallic;
	vec3 irradiance       = texture(u_IrradianceMap, N).rgb;
	vec3 diffuse          = irradiance * albedo;
	vec3 ambient          = (kD * diffuse) * ao;
	
	vec3 color = ambient + Lo;
	o_Color = vec4(color, 1.0);
	
	// POST-PROCESSING:
	// HDR tonemapping
	color = color / (color + vec3(1.0));
	o_Color = vec4(color, 1.0);
}

vec3 GetNormalFromMap()
{
	vec3 tangentNormal = texture(u_Material.NormalMap, Input.TexCoord).xyz * 2.0 - 1.0;
	
	vec3 Q1  = dFdx(Input.Position);
	vec3 Q2  = dFdy(Input.Position);
	vec2 st1 = dFdx(Input.TexCoord);
	vec2 st2 = dFdy(Input.TexCoord);
	
	vec3 n   = normalize(Input.Normal);
	vec3 t   = normalize(Q1 * st2.t - Q2 * st1.t);
	vec3 b   = -normalize(cross(n, t));
	mat3 tbn = mat3(t, b, n);
	
	return normalize(tbn * tangentNormal);
}

float DistributionGGX(vec3 n, vec3 h, float roughness)
{
	float a      = roughness * roughness;
	float a2     = a * a;
	float nDotH  = max(dot(n, h), 0.0);
	float nDotH2 = nDotH * nDotH;
	
	float nom    = a2;
	float denom  = (nDotH2 * (a2 - 1.0) + 1.0);
	denom        = PI * denom * denom;
	
	return nom / denom;
}

float GeometrySchlickGGX(float nDotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;
	
	float nom   = nDotV;
	float denom = nDotV * (1.0 - k) + k;
	
	return nom / denom;
}
  
float GeometrySmith(vec3 n, vec3 v, vec3 l, float roughness)
{
	float nDotV = max(dot(n, v), 0.0);
	float nDotL = max(dot(n, l), 0.0);
	float ggx1  = GeometrySchlickGGX(nDotV, roughness);
	float ggx2  = GeometrySchlickGGX(nDotL, roughness);
	
	return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 f0)
{
	return f0 + (1.0 - f0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}