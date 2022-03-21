#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Normal;
layout(location = 3) in int a_TexIndex;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec2 v_TexCoord;
out vec3 v_Normal;
out int v_TexIndex;

void main()
{
    gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
    
    // Calculate the transformed normal                      w = 0!
    //vec4 transformedModelNormal = normalize(u_Model * vec4(a_Normal, 0.0));
    
    v_TexCoord	= a_TexCoord;
    v_Normal	= mat3(transpose(inverse(u_Model))) * a_Normal;
    v_TexIndex	= a_TexIndex;
}

#type fragment
#version 330 core

uniform sampler2D u_Samplers[32];

in vec2 v_TexCoord;
in vec3 v_Normal;
in int v_TexIndex;

out vec4 o_Color;

void main()
{
    vec4 color = texture(u_Samplers[0], v_TexCoord);
    o_Color = color;
}
