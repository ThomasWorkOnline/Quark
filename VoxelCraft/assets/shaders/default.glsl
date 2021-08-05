#type vertex
#version 410 core

layout(location = 0) in ivec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in float a_Intensity;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform dvec3 u_Position;

out vec2 v_TexCoord;
flat out float v_Intensity;

void main()
{
	// High precision vertex position
	dvec4 position = (dmat4(u_Model) * dvec4(a_Position.xyz, 1.0)) + dvec4(u_Position.xyz, 1.0);
	gl_Position = vec4(dmat4(u_Projection) * dmat4(u_View) * position);

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
