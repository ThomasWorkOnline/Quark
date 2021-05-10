#type vertex
#version 330 core
			
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in float a_TexIndex;
layout(location = 3) in vec3 a_Normal;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 v_Position;
out vec3 v_Normal;
out vec3 v_CameraPosition;

void main()
{
	// Calculate the projected vertex
	vec4 position = u_Model * vec4(a_Position, 1.0);
	gl_Position = u_Projection * u_View * position;

    v_Position = position.xyz;
	v_Normal = normalize(mat3(transpose(inverse(u_Model))) * a_Normal);

    mat4 invView = inverse(u_View);
    v_CameraPosition = vec3(invView[3][0], invView[3][1], invView[3][2]);
}

#type geometry
#version 330 core

layout(triangles) in;
layout(line_strip, max_vertices = 14) out;

uniform float u_NormalLength;
uniform mat4 u_View;
uniform mat4 u_Projection;

in vec3 v_Position[];
in vec3 v_Normal[];
in vec3 v_CameraPosition[];

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

    // Center of the triangle
    vec3 facePosition = (v_Position[0] + v_Position[1] + v_Position[2]) / gl_in.length();
    vec3 faceNormal = normalize(v_Normal[0] + v_Normal[1] + v_Normal[2]);
  
    gl_Position = viewProjection * vec4(facePosition, 1.0);
    v_Color = vec4(0.0, 1.0, 0.0, 1.0);
    EmitVertex();
  
    gl_Position = viewProjection * vec4(facePosition + faceNormal * u_NormalLength, 1.0);
    v_Color = vec4(0.0, 1.0, 0.0, 1.0);
    EmitVertex();

    EndPrimitive();

    // Edge normals
    for (int i = 0; i < gl_in.length(); i++)
    {
        gl_Position = viewProjection * vec4(v_Position[i], 1.0);
        v_Color = vec4(1.0, 1.0, 0.0, 1.0);
        EmitVertex();

        gl_Position = viewProjection * vec4(v_Position[i] + v_Normal[i] * u_NormalLength * 0.5, 1.0);
        v_Color = vec4(1.0, 1.0, 0.0, 1.0);
        EmitVertex();

        EndPrimitive();
    }
}

#type fragment
#version 330 core

in vec4 v_Color;

void main()
{
	gl_FragColor = v_Color;
}