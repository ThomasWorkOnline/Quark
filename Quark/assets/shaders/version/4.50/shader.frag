#version 450

layout(location = 0) in vec3 VertexOutput_FragColor;
layout(location = 0) out vec4 o_OutColor;

void main()
{
    o_OutColor = vec4(VertexOutput_FragColor, 1.0);
}
