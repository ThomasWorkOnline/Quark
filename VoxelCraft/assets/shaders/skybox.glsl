#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;

out vec3 v_TexCoords;

uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
	v_TexCoords = a_Position;
	vec4 position = u_Projection * u_View * vec4(a_Position, 1.0);
	gl_Position = position.xyww;
}

#type fragment
#version 330 core

uniform samplerCube u_Cubemap;

in vec3 v_TexCoords;

void main()
{
	gl_FragColor = texture(u_Cubemap, v_TexCoords);
}
