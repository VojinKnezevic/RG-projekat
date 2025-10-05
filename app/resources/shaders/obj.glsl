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

    vec3 diffuseColor = texture(material.diffuse, TexCoords).rgb;

    // OBJ files often have actual textures or white fallbacks
    // Check for white/invalid textures and provide good car colors
    float brightness = dot(diffuseColor, vec3(0.299, 0.587, 0.114));
    bool isWhiteOrInvalid = brightness > 0.9 ||
                           (diffuseColor.r > 0.9 && diffuseColor.g > 0.9 && diffuseColor.b > 0.9) ||
                           (diffuseColor.r > 0.8 && diffuseColor.g < 0.2 && diffuseColor.b > 0.8);

    if (isWhiteOrInvalid) {
        // Use red color for Alfa Romeo
        diffuseColor = vec3(0.8, 0.1, 0.1);
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
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 diffuseColor = texture(material.diffuse, TexCoords).rgb;

    // Same texture validation as directional light
    float brightness = dot(diffuseColor, vec3(0.299, 0.587, 0.114));
    bool isWhiteOrInvalid = brightness > 0.9 ||
                           (diffuseColor.r > 0.9 && diffuseColor.g > 0.9 && diffuseColor.b > 0.9) ||
                           (diffuseColor.r > 0.8 && diffuseColor.g < 0.2 && diffuseColor.b > 0.8);

    if (isWhiteOrInvalid) {
        // Use red color for Alfa Romeo
        diffuseColor = vec3(0.8, 0.1, 0.1);
    }

    vec3 ambient = light.color * 0.1 * diffuseColor;
    vec3 diffuse = light.color * diff * diffuseColor;

    return (ambient + diffuse) * attenuation;
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