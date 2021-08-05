#type vertex
#version 410 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform dvec3 u_Position;

void main()
{
	// High precision vertex position
	dvec4 position = (dmat4(u_Model) * dvec4(a_Position.xyz, 1.0)) + dvec4(u_Position.xyz, 0.0);
	gl_Position = vec4(dmat4(u_Projection) * dmat4(u_View) * position);

	// Calculate the transformed normal                      w = 0!
	//vec4 transformedModelNormal = normalize(u_Model * vec4(a_Normal, 0.0));

	//v_TexCoord = a_TexCoord;
	//v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;
}

#type fragment
#version 410 core

void main()
{
    gl_FragColor = vec4(1.0);
}
