#version 450

layout(binding = 0) uniform sampler2D u_Samplers[32];

layout(location = 0) in VertexOutput
{
	vec2 TexCoord;
	vec4 Tint;
	flat int TexIndex;
} v_Input;

layout(location = 0) out vec4 o_Color;

void main()
{
	vec4 color = texture(u_Samplers[v_Input.TexIndex], v_Input.TexCoord.xy);
	o_Color = color * v_Input.Tint;
}
