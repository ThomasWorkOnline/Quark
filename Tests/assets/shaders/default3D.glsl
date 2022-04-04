#type vertex
#version 420 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Normal;

layout(std140, binding = 0) uniform Camera
{
    mat4 u_ViewProjection;
};

uniform mat4 u_Model;

out VertexOutput
{
    vec2 TexCoord;
    vec3 Normal;
} v_Output;

void main()
{
    gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
    
    // Calculate the transformed normal                      w = 0!
    //vec4 transformedModelNormal = normalize(u_Model * vec4(a_Normal, 0.0));
    
    v_Output.TexCoord = a_TexCoord;
    v_Output.Normal   = mat3(transpose(inverse(u_Model))) * a_Normal;
}

#type fragment
#version 330 core

vec3 lightDir = normalize(vec3(1.0, 0.3, 0.5));
float ambiant = 0.1;

uniform sampler2D u_Sampler;

in VertexOutput
{
    vec2 TexCoord;
    vec3 Normal;
} v_Input;

out vec4 o_Color;

void main()
{
    vec4 color = texture(u_Sampler, v_Input.TexCoord);
    o_Color = vec4(color.rgb * max(dot(lightDir, v_Input.Normal), 0.0) + ambiant, color.a);
}
