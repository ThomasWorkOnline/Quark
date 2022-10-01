#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec4 a_Tint;
layout(location = 3) in int  a_TexIndex;

layout(std140) uniform Camera {
	mat4 ViewProjection;
} u_Camera;

out vec2 v_TexCoord;
out vec4 v_Tint;
flat out int v_TexIndex;

void main()
{
	gl_Position = u_Camera.ViewProjection * vec4(a_Position, 1.0);
	v_TexCoord = a_TexCoord;
	v_Tint     = a_Tint;
	v_TexIndex = a_TexIndex;
}
