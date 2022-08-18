#version 450 core

layout(binding = 0) uniform sampler2D u_Samplers[32];

layout(location = 0) in VertexOutput {
	vec2 TexCoord;
	vec4 Color;
	flat int TexIndex;
} Input;

layout(location = 0) out vec4 o_Color;

void main()
{
	// Glyph information is encoded in the red channel
	float texel = texture(u_Samplers[Input.TexIndex], Input.TexCoord.xy, 0).r;

	o_Color = vec4(Input.Color.rgb, Input.Color.a * texel);
}
