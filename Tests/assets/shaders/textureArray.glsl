#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in int  a_TexIndex;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out VertexOuput
{
    vec2 TexCoord;
    flat int TexIndex;
} v_Output;

void main()
{
    gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
    
    v_Output.TexCoord = a_TexCoord;
    v_Output.TexIndex = a_TexIndex;
}

#type fragment
#version 330 core

uniform sampler2DArray u_Sampler;

in VertexOuput
{
    vec2 TexCoord;
    flat int TexIndex;
} v_Input;

out vec4 o_Color;

void main()
{
    vec4 texel = texture(u_Sampler, vec3(v_Input.TexCoord, v_Input.TexIndex));
    o_Color = texel;
}
