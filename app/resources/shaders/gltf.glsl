//#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 aInstanceMatrix;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform bool useInstancing;

void main()
{
    mat4 modelMatrix = useInstancing ? aInstanceMatrix : model;

    FragPos = vec3(modelMatrix * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(modelMatrix))) * aNormal;
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

struct SpotLight {
    bool enabled;
    vec3 position;
    vec3 direction;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;
};

#define MAX_POINT_LIGHTS 4
#define MAX_SPOT_LIGHTS 2

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;
uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 diffuseColor = texture(material.diffuse, TexCoords).rgb;
    vec3 specularColor = texture(material.specular, TexCoords).rgb;

    bool hasInvalidDiffuse = (diffuseColor.r > 0.8 && diffuseColor.g < 0.2 && diffuseColor.b > 0.8) ||
                            (diffuseColor.g > 0.8 && diffuseColor.r < 0.2 && diffuseColor.b < 0.2) ||
                            (diffuseColor.r > 0.95 && diffuseColor.g > 0.95 && diffuseColor.b > 0.95);

    if (hasInvalidDiffuse) {
        diffuseColor = vec3(0.6, 0.6, 0.7); // Use bluish-gray for better visibility
    }

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

    bool hasInvalidDiffuse = (diffuseColor.r > 0.8 && diffuseColor.g < 0.2 && diffuseColor.b > 0.8) ||
                            (diffuseColor.g > 0.8 && diffuseColor.r < 0.2 && diffuseColor.b < 0.2) ||
                            (diffuseColor.r > 0.95 && diffuseColor.g > 0.95 && diffuseColor.b > 0.95);

    if (hasInvalidDiffuse) {
        diffuseColor = vec3(0.6, 0.6, 0.7); // Use bluish-gray for better visibility
    }

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

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    if (!light.enabled) {
        return vec3(0.0);
    }

    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);

    // Spotlight calculation
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = cos(radians(light.cutOff)) - cos(radians(light.outerCutOff));
    float intensity = clamp((theta - cos(radians(light.outerCutOff))) / epsilon, 0.0, 1.0);

    float diff = max(dot(normal, lightDir), 0.0);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 diffuseColor = texture(material.diffuse, TexCoords).rgb;
    vec3 specularColor = texture(material.specular, TexCoords).rgb;

    // Check for invalid/missing textures
    bool hasInvalidDiffuse = (diffuseColor.r > 0.8 && diffuseColor.g < 0.2 && diffuseColor.b > 0.8) ||
                            (diffuseColor.g > 0.8 && diffuseColor.r < 0.2 && diffuseColor.b < 0.2) ||
                            (diffuseColor.r > 0.95 && diffuseColor.g > 0.95 && diffuseColor.b > 0.95);

    if (hasInvalidDiffuse) {
        diffuseColor = vec3(0.6, 0.6, 0.7);
    }

    float specularIntensity = dot(specularColor, vec3(0.299, 0.587, 0.114));
    bool hasGreenArtifact = (specularColor.g > specularColor.r * 1.5) && (specularColor.g > specularColor.b * 1.5);
    bool isLowIntensity = specularIntensity < 0.15;
    bool hasExtremeValues = any(greaterThan(specularColor, vec3(1.5))) || any(lessThan(specularColor, vec3(0.0)));

    if (hasGreenArtifact || isLowIntensity || hasExtremeValues) {
        specularColor = vec3(0.8);
    }

    vec3 ambient = light.color * 0.1 * diffuseColor;
    vec3 diffuse = light.color * diff * diffuseColor;

    return (ambient + diffuse) * attenuation * intensity;
}

void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = CalcDirLight(dirLight, norm, viewDir);

    for (int i = 0; i < MAX_POINT_LIGHTS; i++) {
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }

    for (int i = 0; i < MAX_SPOT_LIGHTS; i++) {
        result += CalcSpotLight(spotLights[i], norm, FragPos, viewDir);
    }

    FragColor = vec4(result, 1.0);
}
