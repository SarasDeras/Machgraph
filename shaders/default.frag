#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float     shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};


uniform vec3 viewPos;

uniform Material material;
uniform Light light;

uniform samplerCube depthMap;
uniform float far_plane;

float shadow_calc(vec3 fragPos){
    // получаем вектор между положением фрагмента и положением источника света
    vec3 fragToLight = fragPos - light.position;
    // используем полученный вектор для выборки из карты глубины
    float closestDepth = texture(depthMap, normalize(fragToLight)).r;
    // теперь получим текущую линейную глубину как длину между фрагментом и положением источника света
    float currentDepth = length(fragToLight);
    // теперь проводим проверку на нахождение в тени
    float bias = 0.0;
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;
    // отладка - отображение значений переменной  closestDepth (для визуализации кубической карты глубины)
    //FragColor = vec4(vec3(shadow), 1.0);
    return shadow;
}

void main()
{
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
        		    light.quadratic * (distance * distance));

    // ambient
    vec3 ambient =  vec3(texture(material.diffuse, TexCoord)) * light.ambient;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (diff * vec3(texture(material.diffuse, TexCoord))) * light.diffuse;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    //vec3 reflectDir = reflect(-lightDir, norm);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular  * (spec * vec3(texture(material.specular, TexCoord)));

    diffuse  *= attenuation;
    specular *= attenuation;

    float shadow = shadow_calc(FragPos);
    //shadow = 0;
    vec3 result = ambient + (1.0 - shadow) * (diffuse + specular);
    //vec3 result = ambient + diffuse + specular;
    //vec3 result = ambient;

    // Применяем гамма-коррекцию
    float gamma = 2.2;
    FragColor.rgb = pow(result.rgb, vec3(1.0/gamma));
}

