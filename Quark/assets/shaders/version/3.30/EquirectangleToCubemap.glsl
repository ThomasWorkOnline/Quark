#type vertex
#version 450 core

layout (location = 0) in vec3 a_Position;

uniform mat4 u_View;
uniform mat4 u_Projection;

layout(location = 0) out VertexOutput {
    vec3 Position;
} Output;

void main()
{
    gl_Position = u_Projection * u_View * vec4(a_Position, 1.0);
    Output.Position.x =  a_Position.x;
    Output.Position.y = -a_Position.y;
    Output.Position.z =  a_Position.z;
}

#type fragment
#version 450 core

layout(location = 0) in VertexOutput {
    vec3 Position;
} Input;

uniform sampler2D u_EquirectangularMap;

layout(location = 0) out vec4 o_Color;

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
    vec2 uv = SampleSphericalMap(normalize(Input.Position));
    vec3 color = texture(u_EquirectangularMap, uv).rgb;

    o_Color = vec4(color, 1.0);
}
