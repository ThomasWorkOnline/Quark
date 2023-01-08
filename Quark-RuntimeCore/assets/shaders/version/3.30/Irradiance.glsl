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
    gl_Position = u_Projection * u_View * vec4(a_Position, 1.0);
    Output.Position = a_Position;
}

#type fragment
#version 450 core

layout(location = 0) in VertexOutput {
    vec3 Position;
} Input;

uniform samplerCube u_EnvironmentMap;

const float PI = 3.14159265359;

layout(location = 0) out vec4 o_Color;

void main()
{		
	// The world vector acts as the normal of a tangent surface
    // from the origin, aligned to Input.Position. Given this normal, calculate all
    // incoming radiance of the environment. The result of this radiance
    // is the radiance of light coming from -Normal direction, which is what
    // we use in the PBR shader to sample irradiance.
    vec3 N = normalize(Input.Position);

    vec3 irradiance = vec3(0.0);   
    
    // tangent space calculation from origin point
    vec3 up    = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, N));
    up         = normalize(cross(N, right));
       
    float sampleDelta = 0.025;
    unsigned int nrSamples = 0u;
    for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            // spherical to cartesian (in tangent space)
            vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N; 

            irradiance += texture(u_EnvironmentMap, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }

    irradiance = PI * irradiance * (1.0 / float(nrSamples));
    o_Color = vec4(irradiance, 1.0);
}
