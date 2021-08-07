#type vertex
#version 410 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in float a_Intensity;

//uniform mat4 u_Model; // Unused
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform dvec3 u_CameraPosition;
uniform ivec3 u_ModelOffset;

out vec2 v_TexCoord;
flat out float v_Intensity;

void main()
{
	vec4 modelDistance = vec4(-u_CameraPosition + u_ModelOffset, 0.0);
	vec4 position = /*u_Model * */ vec4(a_Position, 1.0) + modelDistance;
	gl_Position = u_Projection * u_View * position;

	v_TexCoord = a_TexCoord;
	v_Intensity = a_Intensity;
}

#type fragment
#version 410 core

#define SPRITE_SHEET 0

in vec2 v_TexCoord;
flat in float v_Intensity;

uniform sampler2D u_Samplers[32];

void main()
{
	vec4 texture = texture(u_Samplers[SPRITE_SHEET], v_TexCoord);
	if (texture.a < 0.3)
		discard;

    gl_FragColor = vec4(texture.xyz * v_Intensity, 1.0);
}
