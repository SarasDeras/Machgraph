#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
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

uniform Light light;
uniform Material material;

uniform vec3 viewPos;

uniform float far_plane;
uniform bool shadows;

float ShadowCalculation(vec3 fragPos) {
    // получаем вектор между положением фрагмента и положением источника света
    vec3 fragToLight = fragPos - light.position;
    // используем полученный вектор для выборки из карты глубины    
    float closestDepth = texture(depthMap, fragToLight).r;
    // в данный момент значения лежат в диапазоне [0,1]. Преобразуем их обратно к исходным значениям
    closestDepth *= far_plane;
    // теперь получим текущую линейную глубину как длину между фрагментом и положением источника света
    float currentDepth = length(fragToLight);
    // теперь проводим проверку на нахождение в тени
    float bias = 0.05;
    float shadow  = 0.0;
    float samples = 4.0;
    float offset  = 0.1;
    for(float x = -offset; x < offset; x += offset / (samples * 0.5)) {
        for(float y = -offset; y < offset; y += offset / (samples * 0.5)) {
            for(float z = -offset; z < offset; z += offset / (samples * 0.5)) {
                float closestDepth = texture(depthMap, fragToLight + vec3(x, y, z)).r;
                closestDepth *= far_plane;
                if(currentDepth - bias > closestDepth)
                    shadow += 1.0;
            }
        }
    }
    shadow /= (samples * samples * samples);
    return shadow;
}

void main()
{
    float distance = length(light.position - fs_in.FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
                		    light.quadratic * (distance * distance));

    vec3 color = texture(material.diffuse, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);

    // ambient
    vec3 ambient = vec3(texture(material.diffuse, fs_in.TexCoords)) * light.ambient ;

    // diffuse
    vec3 norm = normalize(fs_in.Normal);
    vec3 lightDir = normalize(light.position - fs_in.FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (diff * vec3(texture(material.diffuse, fs_in.TexCoords))) * light.diffuse;

    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec =  pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    //vec3 specular = light.specular * spec * vec3(1.0f);

    vec3 specular = light.specular  * spec * vec3(texture(material.specular, fs_in.TexCoords));


    diffuse  *= attenuation;
    specular *= attenuation;

    // вычисляем тень
    float shadow = shadows ? ShadowCalculation(fs_in.FragPos) : 0.0;
    vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular));
    //vec3 result = ambient + (1.0 - shadow) * diffuse;

    // Применяем гамма-коррекцию
    float gamma = 2.2;
    FragColor.rgb = pow(result.rgb, vec3(1.0/gamma));
    //FragColor = vec4(1.0f);
}

