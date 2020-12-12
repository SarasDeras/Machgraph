#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
} fs_in;

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

uniform samplerCube depthMap;

uniform Material material;
uniform Light light;

uniform vec3 viewPos;

uniform float far_plane;
uniform bool shadows;

float shadow_calc(vec3 fragPos) {
    // получаем вектор между положением фрагмента и положением источника света
    vec3 fragToLight = fragPos - light.position;
    // используем полученный вектор для выборки из карты глубины
    float closestDepth = texture(depthMap, fragToLight).r;
    // в данный момент значения лежат в диапазоне [0,1]. Преобразуем их обратно к исходным значениям
    closestDepth *= far_plane;
    // теперь получим текущую линейную глубину как длину между фрагментом и положением источника света
    float currentDepth = length(fragToLight);
    // теперь проводим проверку на нахождение в тени
    float bias = 0.05; // мы используем гораздо большее теневое смещение, так как значение глубины теперь находится в  диапазоне [near_plane, far_plane]
    //float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;
    float shadow = 0.0;
    if (currentDepth > closestDepth + bias){
        shadow = 1.0;
    }
    // отладка - отображение значений переменной  closestDepth (для визуализации кубической карты глубины)
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);
    return shadow;
}

void main2() {
    float distance = length(light.position - fs_in.FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
            		    light.quadratic * (distance * distance));

    // ambient
    vec3 ambient = vec3(texture(material.diffuse, fs_in.TexCoord));

    // diffuse
    vec3 norm = normalize(fs_in.Normal);
    vec3 lightDir = normalize(light.position - fs_in.FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (diff * vec3(texture(material.diffuse, fs_in.TexCoord))) * light.diffuse;

    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * vec3(texture(material.specular, fs_in.TexCoord)));

    diffuse  *= attenuation;
    specular *= attenuation;

    //float shadow = shadow_calc(fs_in.FragPos);
    vec3 result = ambient + (diffuse + specular);
    //vec3 result = ambient;

    // Применяем гамма-коррекцию
    float gamma = 2.2;
    FragColor.rgb = pow(result.rgb, vec3(1.0/gamma));
    //FragColor = vec4(1.0f);
}

void main() {
    vec3 color = texture(material.diffuse, fs_in.TexCoord).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.3);
    // фоновая составляющая
    vec3 ambient = 0.3 * color;
    // диффузная составляющая
    vec3 lightDir = normalize(light.position - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // отраженная составляющая
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;
    // вычисляем тень
    float shadow = shadows ? shadow_calc(fs_in.FragPos) : 0.0;
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    FragColor = vec4(color, 1.0);
    FragColor.rgb = pow(result.rgb, vec3(1.0/gamma));
}


