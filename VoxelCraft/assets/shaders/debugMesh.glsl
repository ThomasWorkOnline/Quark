#type vertex
#version 410 core
			
layout(location = 0) in ivec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in float a_Intensity;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform dvec3 u_Position;

out vec3 v_Position;

void main()
{
    // High precision vertex position
	dvec4 position = (dmat4(u_Model) * dvec4(a_Position.xyz, 1.0)) + dvec4(u_Position.xyz, 1.0);
    gl_Position = vec4(dmat4(u_Projection) * dmat4(u_View) * position);

	//vec4 position = u_Model * vec4(a_Position.xyz, 1.0);
	//gl_Position = u_Projection * u_View * position;

    v_Position = vec3(position.xyz);
}

#type geometry
#version 410 core

layout(triangles) in;
layout(line_strip, max_vertices = 14) out;

uniform float u_NormalLength;
uniform mat4 u_View;
uniform mat4 u_Projection;

in vec3 v_Position[];

out vec4 v_Color;

void main()
{
    mat4 viewProjection = u_Projection * u_View;

    // Skeleton
	gl_Position = gl_in[0].gl_Position;
    v_Color = vec4(1.0, 1.0, 1.0, 1.0);
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    v_Color = vec4(1.0, 1.0, 1.0, 1.0);
    EmitVertex();

	EndPrimitive();

	gl_Position = gl_in[1].gl_Position;
    v_Color = vec4(1.0, 1.0, 1.0, 1.0);
    EmitVertex();

	gl_Position = gl_in[2].gl_Position;
    v_Color = vec4(1.0, 1.0, 1.0, 1.0);
    EmitVertex();

	EndPrimitive();

	gl_Position = gl_in[2].gl_Position;
    v_Color = vec4(1.0, 1.0, 1.0, 1.0);
    EmitVertex();

	gl_Position = gl_in[0].gl_Position;
    v_Color = vec4(1.0, 1.0, 1.0, 1.0);
    EmitVertex();

    EndPrimitive();
}

#type fragment
#version 410 core

in vec4 v_Color;

void main()
{
	gl_FragColor = v_Color;
}
