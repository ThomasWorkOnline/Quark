#type vertex
#version 410 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

uniform mat4 u_Model;
uniform mat4 u_Projection;
uniform mat4 u_View;

out vec2 v_TexCoord;
out vec3 v_Color;
flat out int v_TexIndex;

void main()
{
	gl_Position = u_Projection * u_View * vec4(a_Position, 1.0);

	v_TexCoord	= a_TexCoord;
	v_Color		= vec3(1.0, 1.0, 0.0);
	v_TexIndex	= 0;
}

#type fragment
#version 410 core

in vec2 v_TexCoord;
in vec3 v_Color;
flat in int v_TexIndex;

uniform sampler2D u_Samplers[32];

void main()
{
	float texture = texture(u_Samplers[v_TexIndex], v_TexCoord).r;
	vec4 sampled = vec4(v_Color, texture);
    gl_FragColor = sampled;
}
