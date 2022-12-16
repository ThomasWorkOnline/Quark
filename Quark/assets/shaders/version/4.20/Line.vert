#version 420 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;

layout(push_constant) uniform Camera {
	mat4 ViewProjection;
} u_Camera;

layout(location = 0) out vec4 o_Color;

void main()
{
	gl_Position = u_Camera.ViewProjection * vec4(a_Position, 1.0);
	o_Color = a_Color;
}
