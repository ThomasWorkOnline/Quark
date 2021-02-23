#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Normal;
layout(location = 3) in float a_TexIndex;

uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 v_Position;
out vec2 v_TexCoord;
out vec3 v_Normal;
flat out float v_TexIndex;

void main()
{
	vec4 position = vec4(a_Position, 1.0);

	gl_Position = u_Projection * u_View * position;

	v_Position = position.xyz;
	v_TexCoord = a_TexCoord;
	v_Normal = a_Normal;
    v_TexIndex = a_TexIndex;
}

#type fragment
#version 330 core

uniform sampler2D u_Samplers[32];

in vec3 v_Position;
in vec2 v_TexCoord;
in vec3 v_Normal;
flat in float v_TexIndex;

void main()
{
	vec4 color = texture(u_Samplers[int(v_TexIndex)], v_TexCoord);
	if (color.a < 1)
		discard;
    gl_FragColor = color;
}
