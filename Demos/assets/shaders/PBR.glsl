#type vertex
#version 420 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Normal;

layout(std140, binding = 0) uniform Camera
{
    mat4 u_ViewProjection;
};

uniform mat4 u_Model;

out VertexOutput
{
    vec3 Position;
    vec2 TexCoord;
    vec3 Normal;
} v_Output;

void main()
{
    vec4 wsPosition = u_Model * vec4(a_Position, 1.0);
    gl_Position = u_ViewProjection * wsPosition;
    
    v_Output.Position = wsPosition.xyz;
    v_Output.TexCoord = a_TexCoord;
    v_Output.Normal   = mat3(u_Model) * a_Normal;
}

#type fragment
#version 330 core

const float PI = 3.14159265359;

vec3 GetNormalFromMap();
float DistributionGGX(vec3 n, vec3 h, float roughness);
float GeometrySchlickGGX(float nDotV, float k);
float GeometrySmith(vec3 n, vec3 v, vec3 l, float k);
vec3 FresnelSchlick(float cosTheta, vec3 f0);

in VertexOutput
{
    vec3 Position;
    vec2 TexCoord;
    vec3 Normal;
} v_Input;

uniform sampler2D u_AlbedoMap;
uniform sampler2D u_NormalMap;
uniform sampler2D u_MetallicMap;
uniform sampler2D u_RoughnessMap;
uniform sampler2D u_AmbiantOcclusionMap;

uniform vec3  u_CameraPos;
uniform vec3  u_Albedo;
uniform float u_Metallic;
uniform float u_Roughness;
uniform float u_AmbiantOcclusion;

uniform vec3 u_LightPositions[4];
uniform vec3 u_LightColors[4];

out vec4 o_Color;

// Shader source from learnopengl:
// https://learnopengl.com/code_viewer_gh.php?code=src/6.pbr/1.2.lighting_textured/1.2.pbr.fs
void main()
{
    vec3 albedo     = texture(u_AlbedoMap,    v_Input.TexCoord).rgb;
    float metallic  = texture(u_MetallicMap,  v_Input.TexCoord).r;
    float roughness = texture(u_RoughnessMap, v_Input.TexCoord).r;
    float ao        = 0.6;

    vec3 N = GetNormalFromMap();
    vec3 V = normalize(u_CameraPos - v_Input.Position);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 4; ++i) 
    {
        // calculate per-light radiance
        vec3 L = normalize(u_LightPositions[i] - v_Input.Position);
        vec3 H = normalize(V + L);
        float distance = length(u_LightPositions[i] - v_Input.Position);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = u_LightColors[i] * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);
        vec3 F    = FresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   
    
    // ambient lighting (note that the next IBL tutorial will replace
    // this ambient lighting with environment lighting).
    vec3 ambient = vec3(0.03) * albedo * ao;
    
    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));

    o_Color = vec4(color, 1.0);
}

vec3 GetNormalFromMap()
{
    vec3 tangentNormal = texture(u_NormalMap, v_Input.TexCoord).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(v_Input.Position);
    vec3 Q2  = dFdy(v_Input.Position);
    vec2 st1 = dFdx(v_Input.TexCoord);
    vec2 st2 = dFdy(v_Input.TexCoord);

    vec3 n   = normalize(v_Input.Normal);
    vec3 t   = normalize(Q1 * st2.t - Q2 * st1.t);
    vec3 b   = -normalize(cross(n, t));
    mat3 tbn = mat3(t, b, n);

    return normalize(tbn * tangentNormal);
}

float DistributionGGX(vec3 n, vec3 h, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float nDotH = max(dot(n, h), 0.0);
    float nDotH2 = nDotH * nDotH;

    float nom   = a2;
    float denom = (nDotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

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
  
float GeometrySmith(vec3 n, vec3 v, vec3 l, float k)
{
    float nDotV = max(dot(n, v), 0.0);
    float nDotL = max(dot(n, l), 0.0);
    float ggx1 = GeometrySchlickGGX(nDotV, k);
    float ggx2 = GeometrySchlickGGX(nDotL, k);
	
    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 f0)
{
    return f0 + (1.0 - f0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
