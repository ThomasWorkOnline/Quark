#type vertex
#version 330 core
layout (location = 0) in vec3 a_Position;

uniform mat4 u_Projection;
uniform mat4 u_View;

out VertexOutput
{
    vec3 Position;
} v_Output;

void main()
{
    mat4 rotView = mat4(mat3(u_View));
    vec4 clipPos = u_Projection * rotView * vec4(a_Position, 1.0);

    gl_Position = clipPos.xyww;
    v_Output.Position = a_Position;
}

#type fragment
#version 330 core

in VertexOutput
{
    vec3 Position;
} v_Input;
  
uniform samplerCube u_EnvironmentMap;

out vec4 o_Color;
  
void main()
{
    vec3 envColor = texture(u_EnvironmentMap, v_Input.Position).rgb;
    envColor = envColor / (envColor + vec3(1.0));
  
    o_Color = vec4(envColor, 1.0);
}
