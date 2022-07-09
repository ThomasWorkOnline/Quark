#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Color;

layout(location = 0) out vec3 v_FragColor;

layout(binding = 0) uniform Camera
{
    mat4 View;
    mat4 Projection;
} u_Camera;

void main()
{
    gl_Position = u_Camera.Projection * u_Camera.View * vec4(a_Position, 1.0);
    v_FragColor = a_Color;
}
