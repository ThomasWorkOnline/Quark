#type vertex
#version 450 core

layout (location = 0) in vec3 a_Position;

uniform mat4 u_View;
uniform mat4 u_Projection;

layout(location = 0) out VertexOutput {
    vec3 Position;
} Output;

void main()
{
    vec4 clipPos = u_Projection * u_View * vec4(a_Position, 1.0);

    gl_Position = clipPos.xyww;
    Output.Position = a_Position;
}

#type fragment
#version 450 core

layout(location = 0) in VertexOutput {
    vec3 Position;
} Input;
  
uniform samplerCube u_EnvironmentMap;
uniform float u_Exposure;

layout(location = 0) out vec4 o_Color;
  
void main()
{
    vec3 envColor = texture(u_EnvironmentMap, Input.Position).rgb;
    envColor = envColor / (envColor + vec3(1.0));
  
    o_Color = vec4(envColor * u_Exposure, 1.0);
    o_Color = vec4(Input.Position, 1.0);
}
