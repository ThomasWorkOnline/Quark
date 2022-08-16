#version 420 core

layout(binding = 0) uniform sampler2D u_Samplers[32];

struct VertexOutput
{
	vec2 TexCoord;
	vec4 Color;
	int TexIndex;
};

layout(location = 0) flat in VertexOutput Input;

layout(location = 0) out vec4 o_Color;

void main()
{
	// Glyph information is encoded in the red channel
	float texel = texture(u_Samplers[Input.TexIndex], Input.TexCoord.xy, 0).r;
				
	if (texel == 0)
		discard;

	o_Color = Input.Color * texel;
}
