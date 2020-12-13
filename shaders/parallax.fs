#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

uniform float heightScale;

vec2 reliefPM(vec2 TexCoords, vec3 ViewDir) {
	float minLayers = 4.0f, maxLayers = 32.0f;
	float numLayers = mix(maxLayers, minLayers, abs(ViewDir).z);

	float deltaStep = 1.0f / numLayers;
	vec2 deltaTexcoords = heightScale * ViewDir.xy / (ViewDir.z * numLayers);

    // Линейный поиск
	vec2 currentTexCoords = TexCoords;
	float currentLayerDepth = 0.;
	float currentDepthValue = texture(depthMap, currentTexCoords).r;
	while (currentDepthValue > currentLayerDepth) {
		currentLayerDepth += deltaStep;
		currentTexCoords -= deltaTexcoords;
		currentDepthValue = texture(depthMap, currentTexCoords).r;
	}
    // Бинарный поиск
    deltaStep *= 0.5;
	deltaTexcoords *= 0.5;
	currentTexCoords += deltaTexcoords;
	currentLayerDepth -= deltaStep;

    int maxfSteps = 5, i = 0;
    while (i < maxfSteps) {
        currentDepthValue = texture(depthMap, currentTexCoords).r;
        deltaStep *= 0.5;
        deltaTexcoords *= 0.5;
        if (currentDepthValue > currentLayerDepth) {
            currentTexCoords -= deltaTexcoords;
            currentLayerDepth += deltaStep;
        } else {
            currentTexCoords += deltaTexcoords;
            currentLayerDepth -= deltaStep;
        }
        i++;
    }
    // Интерполяция не требуется
	return currentTexCoords;
}

void main()
{
    // Параллакс
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec2 texCoords = fs_in.TexCoords;
    
    texCoords = reliefPM(fs_in.TexCoords, viewDir);
    // Отбрасываем лишнее
    if (texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
        discard;

    // Получение нормали из карты нормалей
    vec3 normal = texture(normalMap, texCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    vec3 color = texture(diffuseMap, texCoords).rgb;

    // Упрощённая модель освещения
    // ambient
    vec3 ambient = 0.1 * color;

    // diffuse
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color * vec3(0.5f);

    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = vec3(0.1) * spec;

    vec3 result = ambient + diffuse + specular;

    // Применяем гамма-коррекцию
    float gamma = 2.2;
    FragColor.rgb = pow(result.rgb, vec3(1.0/gamma));
}