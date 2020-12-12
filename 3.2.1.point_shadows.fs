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

float ShadowCalculation(vec3 fragPos)
{
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
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;        
    // отладка - отображение значений переменной  closestDepth (для визуализации кубической карты глубины)
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
        
    return shadow;
}

void main()
{
    vec3 color = texture(material.diffuse, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.3);
    // фоновая составляющая
    vec3 ambient = 0.3 * color;
    // диффузная составляющая
    vec3 lightDir = normalize(light.position- fs_in.FragPos);
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
    float shadow = shadows ? ShadowCalculation(fs_in.FragPos) : 0.0;                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
    FragColor = vec4(lighting, 1.0);
}

