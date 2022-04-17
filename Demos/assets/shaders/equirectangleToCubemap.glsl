#type vertex
#version 420 core
layout (location = 0) in vec3 a_Position;

layout(std140, binding = 0) uniform Camera
{
    mat4 u_ViewProjection;
};

out VertexOutput
{
    vec3 Position;
} v_Output;

void main()
{
    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
    v_Output.Position = a_Position;
}

#type fragment
#version 330 core

in VertexOutput
{
    vec3 Position;
} v_Input;

out vec4 o_Color;

uniform sampler2D u_EquirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{		
    vec2 uv = SampleSphericalMap(normalize(v_Input.Position));
    vec3 color = texture(u_EquirectangularMap, uv).rgb;
    
    o_Color = vec4(color, 1.0);
}
