//#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}

//#shader fragment
#version 330 core

out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    float shininess;
};

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    bool enabled;
    vec3 position;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
};

#define MAX_POINT_LIGHTS 4

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;
uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 diffuseColor = texture(material.diffuse, TexCoords).rgb;
    vec3 specularColor = texture(material.specular, TexCoords).rgb;

    float specularIntensity = dot(specularColor, vec3(0.299, 0.587, 0.114));
    bool hasGreenArtifact = (specularColor.g > specularColor.r * 1.5) && (specularColor.g > specularColor.b * 1.5);
    bool isLowIntensity = specularIntensity < 0.15;
    bool hasExtremeValues = any(greaterThan(specularColor, vec3(1.5))) || any(lessThan(specularColor, vec3(0.0)));

    if (hasGreenArtifact || isLowIntensity || hasExtremeValues) {
        specularColor = vec3(0.8);  // Use neutral gray instead of problematic data
    }

    vec3 ambient = light.ambient * diffuseColor;
    vec3 diffuse = light.diffuse * diff * diffuseColor;

    return (ambient + diffuse);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    if (!light.enabled) {
        return vec3(0.0);
    }

    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 diffuseColor = texture(material.diffuse, TexCoords).rgb;
    vec3 specularColor = texture(material.specular, TexCoords).rgb;

    float specularIntensity = dot(specularColor, vec3(0.299, 0.587, 0.114));
    bool hasGreenArtifact = (specularColor.g > specularColor.r * 1.5) && (specularColor.g > specularColor.b * 1.5);
    bool isLowIntensity = specularIntensity < 0.15;
    bool hasExtremeValues = any(greaterThan(specularColor, vec3(1.5))) || any(lessThan(specularColor, vec3(0.0)));

    if (hasGreenArtifact || isLowIntensity || hasExtremeValues) {
        specularColor = vec3(0.8);
    }

    vec3 ambient = light.color * 0.1 * diffuseColor;
    vec3 diffuse = light.color * diff * diffuseColor;
    vec3 specular = light.color * spec * specularColor;

    return (ambient + diffuse + specular) * attenuation;
}

void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = CalcDirLight(dirLight, norm, viewDir);

    for (int i = 0; i < MAX_POINT_LIGHTS; i++) {
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }

    FragColor = vec4(result, 1.0);
}