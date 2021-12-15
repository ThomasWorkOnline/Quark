#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in int a_TexLayer;
layout(location = 3) in int a_TexIndex;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec2 v_TexCoord;
flat out int v_TexLayer;
flat out int v_TexIndex;

void main()
{
	gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);

	v_TexCoord = a_TexCoord;
	v_TexLayer = a_TexLayer;
	v_TexIndex = a_TexIndex;
}

#type fragment
#version 330 core

uniform sampler2DArray u_Sampler;

in vec2 v_TexCoord;
flat in int v_TexLayer;
flat in int v_TexIndex;

out vec4 o_Color;

void main()
{
	vec4 texel = texture(u_Sampler, vec3(v_TexCoord, v_TexLayer));
    o_Color = texel;
}
