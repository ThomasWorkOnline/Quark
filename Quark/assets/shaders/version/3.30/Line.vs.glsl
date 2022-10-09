#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;

layout(std140) uniform Camera {
	mat4 ViewProjection;
} u_Camera;

out vec4 v_Color;

void main()
{
	gl_Position = u_Camera.ViewProjection * vec4(a_Position, 1.0);
	v_Color = a_Color;
}
