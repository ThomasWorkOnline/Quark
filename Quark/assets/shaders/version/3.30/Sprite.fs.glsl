#version 330 core

uniform sampler2D u_Samplers[16];

in vec2 v_TexCoord;
in vec4 v_Tint;
flat in int v_TexIndex;

out vec4 o_Color;

void main()
{
    vec4 color = texture(u_Samplers[v_TexIndex], v_TexCoord);
    o_Color = color * v_Tint;
}
