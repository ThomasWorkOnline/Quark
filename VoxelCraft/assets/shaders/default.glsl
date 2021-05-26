#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in float a_Intensity;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 v_Position;
out vec2 v_TexCoord;
flat out float v_Intensity;

void main()
{
	vec4 position = u_Model * vec4(a_Position.xyz, 1.0);
	gl_Position = u_Projection * u_View * position;

	v_Position = position.xyz;
	v_TexCoord = a_TexCoord;
	v_Intensity = a_Intensity;
}

#type fragment
#version 330 core

in vec3 v_Position;
in vec2 v_TexCoord;
flat in float v_Intensity;

uniform sampler2D u_Samplers[32];

void main()
{
    gl_FragColor = vec4(texture(u_Samplers[0], v_TexCoord).xyz * v_Intensity, 1.0);
}
