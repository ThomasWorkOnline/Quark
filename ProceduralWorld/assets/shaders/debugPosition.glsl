#type vertex
#version 330 core
			
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Normal;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec4 v_Color;

void main()
{
	// Calculate the projected vertex
	gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);

	v_Color = u_Model * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core
			
in vec4 v_Color;

void main()
{
	gl_FragColor = v_Color;
}
