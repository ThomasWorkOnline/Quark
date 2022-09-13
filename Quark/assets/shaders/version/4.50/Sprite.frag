#version 450 core

layout(set = 0, binding = 1) uniform sampler2D u_Samplers[2];

layout(location = 0) in VertexOutput {
	vec2 TexCoord;
	vec4 Tint;
	flat int TexIndex;
} Input;

layout(location = 0) out vec4 o_Color;

void main()
{
	vec4 color = texture(u_Samplers[Input.TexIndex], Input.TexCoord.xy);
	o_Color = Input.Tint * color;
}
