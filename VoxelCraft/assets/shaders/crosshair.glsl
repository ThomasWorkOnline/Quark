#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;

uniform mat4 u_Projection;

void main()
{
	vec4 position = vec4(a_Position.xyz, 1.0);
	gl_Position = u_Projection * position;
}

#type fragment
#version 330 core

uniform vec4 u_Color;

void main()
{
    gl_FragColor = u_Color;
}
