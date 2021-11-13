#type vertex
#version 410 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Normal;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform dvec3 u_CameraPosition;
uniform ivec3 u_ModelOffset;

void main()
{
	vec4 modelPosition = vec4(-u_CameraPosition + u_ModelOffset, 0.0);
	vec4 position = u_Model * vec4(a_Position, 1.0) + modelPosition;
	gl_Position = u_Projection * u_View * position;

	// Calculate the transformed normal                      w = 0!
	//vec4 transformedModelNormal = normalize(u_Model * vec4(a_Normal, 0.0));

	//v_TexCoord = a_TexCoord;
	//v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;
}

#type fragment
#version 410 core

out vec4 r_Color;

void main()
{
    r_Color = vec4(0.2, 1.0, 0.1, 1.0);
}
