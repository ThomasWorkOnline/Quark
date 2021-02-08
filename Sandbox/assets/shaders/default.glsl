#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Normal;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 v_Position;
out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_CameraPosition;

void main()
{
	vec4 position = u_Model * vec4(a_Position.xyz, 1.0);

	gl_Position = u_Projection * u_View * position;
	// Calculate the transformed normal                      w = 0!
	vec4 transformedModelNormal = normalize(u_Model * vec4(a_Normal, 0.0));

	v_Position = position.xyz;
	v_TexCoord = a_TexCoord;
	v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;
    mat4 invView = inverse(u_View);
    v_CameraPosition = vec3(invView[3][0], invView[3][1], invView[3][2]);
}

#type fragment
#version 330 core

in vec3 v_Position;
in vec2 v_TexCoord;
in vec3 v_Normal;
in vec3 v_CameraPosition;

struct DirectionalLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
    sampler2D normalMap;
    float shininess;
};

uniform PointLight u_Light;
uniform Material u_Material;

vec3 CalculateDirectionLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);
	// Combine results
    vec3 ambient  = light.ambient  * vec3(texture(u_Material.diffuse, v_TexCoord));
	vec3 diffuse  = light.diffuse  * diff * vec3(texture(u_Material.diffuse, v_TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(u_Material.specular, v_TexCoord));
    return (ambient + diffuse + specular);
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);
    // Attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
  			     light.quadratic * (distance * distance));
    // Combine results
    vec3 ambient  = light.ambient  * vec3(texture(u_Material.diffuse, v_TexCoord));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(u_Material.diffuse, v_TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(u_Material.specular, v_TexCoord));
    return (ambient + diffuse + specular) * attenuation;
}

void main()
{
	vec3 norm = normalize(v_Normal);
	vec3 viewDir = normalize(v_CameraPosition - v_Position);
	vec3 lightDir = normalize(u_Light.position - v_Position);
    vec3 halfwayDir = normalize(lightDir + viewDir);

	// Calculate the distance from the light source
	float distance = length(u_Light.position - v_Position);
	float attenuation = 1.0 / (u_Light.constant + u_Light.linear * distance +
    		    u_Light.quadratic * (distance * distance));

	vec3 ambient = u_Light.ambient * vec3(texture(u_Material.diffuse, v_TexCoord));

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = u_Light.diffuse * diff * vec3(texture(u_Material.diffuse, v_TexCoord));

	float spec = pow(max(dot(norm, halfwayDir), 0.0), u_Material.shininess);
	vec3 specular = u_Light.specular * spec * vec3(texture(u_Material.specular, v_TexCoord));

	vec3 result = (ambient + diffuse + specular) * attenuation;
	gl_FragColor = vec4(result, 1.0);
}
