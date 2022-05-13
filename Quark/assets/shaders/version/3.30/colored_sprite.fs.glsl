#version 330 core

uniform sampler2D u_Samplers[__MAX_SAMPLERS__];

in VertexOutput
{
    vec2 TexCoord;
    vec4 Tint;
    flat int TexIndex;
} v_Input;

out vec4 o_Color;

void main()
{
    int lvalueIndex = v_Input.TexIndex;
    vec4 color = texture(u_Samplers[lvalueIndex], v_Input.TexCoord.xy);
    o_Color = color * v_Input.Tint;
}
