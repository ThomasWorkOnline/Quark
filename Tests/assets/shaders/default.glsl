#type vertex
#version 420 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec4 a_Color;
layout(location = 3) in int  a_TexIndex;

layout(std140, binding = 0) uniform Camera
{
    mat4 u_ViewProjection;
};

uniform mat4 u_Model;

out VertexOutput
{
    vec2 TexCoord;
    vec4 Color;
    flat int TexIndex;
} v_Output;

void main()
{
    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
    
    v_Output.TexCoord = a_TexCoord;
    v_Output.Color    = a_Color;
    v_Output.TexIndex = a_TexIndex;
}

#type fragment
#version 330 core

uniform sampler2D u_Samplers[32];

in VertexOutput
{
    vec2 TexCoord;
    vec4 Color;
    flat int TexIndex;
} v_Input;

out vec4 o_Color;

void main()
{
    float texture = texture(u_Samplers[v_Input.TexIndex], v_Input.TexCoord).r;
    vec4 color = vec4(v_Input.Color) * texture;
    o_Color = color;
}
