#type vertex
#version 410 core
			
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in float a_Intensity;

uniform mat4 u_View;
uniform mat4 u_Projection;
uniform dvec3 u_CameraPosition;
uniform ivec3 u_ModelOffset;

out vec3 v_Position;

void main()
{
    vec4 modelDistance = vec4(-u_CameraPosition + u_ModelOffset, 0.0);
	vec4 position =  vec4(a_Position, 1.0) + modelDistance;
    gl_Position = u_Projection * u_View * position;

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
