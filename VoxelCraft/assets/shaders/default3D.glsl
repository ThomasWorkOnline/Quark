#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Normal;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 v_Position;
out vec2 v_TexCoord;
out vec3 v_Normal;

void main()
{
	vec4 position = u_Model * vec4(a_Position.xyz, 1.0);

	gl_Position = u_Projection * u_View * position;
	// Calculate the transformed normal                      w = 0!
	vec4 transformedModelNormal = normalize(u_Model * vec4(a_Normal, 0.0));

	v_Position = position.xyz;
	v_TexCoord = a_TexCoord;
	v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;
}

#type fragment
#version 330 core

in vec3 v_Position;
in vec2 v_TexCoord;
in vec3 v_Normal;

void main()
{
    //gl_FragColor = vec4(result, 1.0);
    gl_FragColor = vec4(1.0);
}
