#version 450 core

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec4 a_Color;
layout(location = 3) in int  a_TexIndex;

layout(location = 0) out VertexOutput {
	vec2 TexCoord;
	vec4 Color;
	flat int TexIndex;
} Output;

void main()
{
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
	Output.TexCoord = a_TexCoord;
	Output.Color    = a_Color;
	Output.TexIndex = a_TexIndex;
}
