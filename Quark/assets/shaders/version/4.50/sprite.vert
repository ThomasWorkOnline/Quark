#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec4 a_Tint;
layout(location = 3) in int  a_TexIndex;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};

layout(location = 0) out VertexOutput
{
	vec2 TexCoord;
	vec4 Tint;
	flat int TexIndex;
} v_Output;

void main()
{
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
	v_Output.TexCoord = a_TexCoord;
	v_Output.Tint     = a_Tint;
	v_Output.TexIndex = a_TexIndex;
}
