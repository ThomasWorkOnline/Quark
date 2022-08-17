#version 420 core

layout(binding = 0) uniform sampler2D u_Samplers[32];

struct VertexOutput
{
	vec2 TexCoord;
	vec4 Tint;
	int TexIndex;
};

layout(location = 0) flat in VertexOutput Input;
layout(location = 0) out vec4 o_Color;

void main()
{
	vec4 color = texture(u_Samplers[Input.TexIndex], Input.TexCoord.xy);
	o_Color = color * Input.Tint;
}
