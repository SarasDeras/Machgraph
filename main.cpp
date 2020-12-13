#include <iostream>
#include <string>
#include <sstream>
#include <cmath>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#define GLEW_STATIC

#include "libs/include/glew.h"
#include "libs/include/glfw3.h"
#include "libs/include/SOIL.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



using namespace std;

unsigned int load_texture(const string& path, bool correction=true){
    unsigned int texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width_t, height_t;
    unsigned char* image = SOIL_load_image(path.c_str(), &width_t, &height_t, 0, SOIL_LOAD_RGB);

    if (correction){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width_t, height_t, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    }else{
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_t, height_t, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    }
    glGenerateMipmap(GL_TEXTURE_2D);

    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}

unsigned int load_cubetexture(const string& path, const vector<string>& names, bool correction=true) {
    unsigned int texture_cube;
    glGenTextures(1, &texture_cube);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_cube);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    int width_t, height_t;
    for (unsigned int i = 0; i < names.size(); i++) {
        unsigned char* image = SOIL_load_image((path + R"(\)" + names[i]).c_str(), &width_t, &height_t,
                0, SOIL_LOAD_RGB);
        if (correction){
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, width_t, height_t, 0,
                GL_RGB, GL_UNSIGNED_BYTE, image);
        }else{
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width_t, height_t, 0,
                         GL_RGB, GL_UNSIGNED_BYTE, image);
        }
        SOIL_free_image_data(image);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return texture_cube;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void renderScene(const Shader& shader);

// настройки
const unsigned int window_width = 1000;
const unsigned int window_height = 800;
// найстроки режимов отображения
bool shadows = true;
bool shadowsKeyPressed = false;

bool light_mode = false;
bool lightKeyPressed = false;

// камера
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float) window_width / 2.0;
float lastY = (float) window_height / 2.0;
bool firstMouse = true;

// тайминги
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Meshes
vector<TriangleMesh> objects;

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad() {
    if (quadVAO == 0)
    {
        // координаты
        glm::vec3 pos1(-1.0f,  1.0f, 0.0f);
        glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
        glm::vec3 pos3( 1.0f, -1.0f, 0.0f);
        glm::vec3 pos4( 1.0f,  1.0f, 0.0f);
        // текстурные координаты
        glm::vec2 uv1(0.0f, 1.0f);
        glm::vec2 uv2(0.0f, 0.0f);
        glm::vec2 uv3(1.0f, 0.0f);
        glm::vec2 uv4(1.0f, 1.0f);
        // векторы нормалей
        glm::vec3 nm(0.0f, 0.0f, 1.0f);

        // вычисляем касательные/бикасательные векторы обоих треугольников
        glm::vec3 tangent1, bitangent1;
        glm::vec3 tangent2, bitangent2;
        // треугольник 1
        // ----------
        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent1 = glm::normalize(tangent1);

        bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent1 = glm::normalize(bitangent1);

        // треугольник 2
        // ----------
        edge1 = pos3 - pos1;
        edge2 = pos4 - pos1;
        deltaUV1 = uv3 - uv1;
        deltaUV2 = uv4 - uv1;

        f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent2 = glm::normalize(tangent2);


        bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent2 = glm::normalize(bitangent2);


        float quadVertices[] = {
                // координаты            // нормали      // текст. коорд.  // касательные                         // бикасательные
                pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
                pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
                pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

                pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
                pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
                pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
        };
        // конфигурируем VAO плоскости
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

int main() {
    //Инициализация GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    //Cоздание окна
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "MashGraph", nullptr, nullptr);
    if (window == nullptr) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    //Инициализация GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cout << "Failed to initialize GLEW" << endl;
        return -1;
    }

    //Обработчики событий и рабочая область
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glViewport(0, 0, window_width, window_height);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //glfwSetKeyCallback(window, );
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);


    //Обработка глубины
    glEnable(GL_DEPTH_TEST);
    //Обработка граней
    glEnable(GL_CULL_FACE);


    //Шейдеры
    Shader shader(R"(..\shaders\mainshader.vs)", R"(..\shaders\mainshader.fs)");
    Shader light_cube(R"(..\shaders\light.vs)", R"(..\shaders\light.fs)");
    Shader skybox_shader(R"(..\shaders\skybox.vs)", R"(..\shaders\skybox.fs)");
    Shader depthShader(R"(..\shaders\depth.vs)", R"(..\shaders\depth.fs)", R"(..\shaders\depth.gs)");
    Shader parallax_shader(R"(..\shaders\parallax.vs)", R"(..\shaders\parallax.fs)");

    //Примитивы
    float vertices[] = {
            //
            // задняя грань
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // нижняя-левая
            1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // верхняя-правая
            1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // нижняя-правая
            1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // верхняя-правая
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // нижняя-левая
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // верхняя-левая
            // передняя грань
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // нижняя-левая
            1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // нижняя-правая
            1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // верхняя-правая
            1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // верхняя-правая
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // верхняя-левая
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // нижняя-левая
            // грань слева
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // верхняя-правая
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // верхняя-левая
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // нижняя-левая
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // нижняя-левая
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // нижняя-правая
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // верхняя-правая
            // грань справа
            1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // верхняя-левая
            1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // нижняя-правая
            1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // верхняя-правая
            1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // нижняя-правая
            1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // верхняя-левая
            1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // нижняя-левая
            // нижняя грань
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // верхняя-правая
            1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // верхняя-левая
            1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // нижняя-левая
            1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // нижняя-левая
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // нижняя-правая
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // верхняя-правая
            // верхняя грань
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // верхняя-левая
            1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // нижняя-правая
            1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // верхняя-правая
            1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // нижняя-правая
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // верхняя-левая
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // нижняя-левая
    };

    float skyboxVertices[] = {
            // Координаты
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
    };
    // Загрузка текстур
    unsigned int cube_diffuse = load_texture(R"(..\textures\box.png)");
    unsigned int cube_spec = load_texture(R"(..\textures\box_specular.png)", false);
    unsigned int floor_diffuse = load_texture(R"(..\textures\floor.jpg)");
    unsigned int woodTexture = load_texture(R"(..\textures\wood.png)");
    unsigned int par_diffuse = load_texture(R"(..\textures\bricks2.jpg)");
    unsigned int par_normal  = load_texture(R"(..\textures\bricks2_normal.jpg)", false);
    unsigned int par_height  = load_texture(R"(..\textures\bricks2_disp.jpg)", false);


    vector<string> faces = {
            "right.jpg",
            "left.jpg",
            "top.jpg",
            "bottom.jpg",
            "front.jpg",
            "back.jpg"
    };

    vector<glm::vec3> cubes_info = {
            //glm::vec3(4.0f, -3.5f, 0.0), glm::vec3(0.5f),
            glm::vec3(2.0f, 2.0f, 0.5), glm::vec3(0.75f),
            glm::vec3(-3.0f, -1.0f, 0.0), glm::vec3(0.5f),
            //glm::vec3(-1.5f, 1.0f, 1.5), glm::vec3(0.5f),
            glm::vec3(-1.5f, 1.0f, -4.0), glm::vec3(0.75f),
    };


    //Созданние mesheй
    TriangleMesh light(vertices, sizeof(vertices), cube_diffuse);
    TriangleMesh floor(vertices, sizeof(vertices), floor_diffuse);
    TriangleMesh wall(vertices, sizeof(vertices), woodTexture);
    TriangleMesh wall2(vertices, sizeof(vertices), woodTexture);

    for (int i = 0; i < cubes_info.size(); i += 2){
        TriangleMesh cube_default(vertices, sizeof(vertices), cube_diffuse, cube_spec);
        cube_default.translate(cubes_info[i]);
        cube_default.scale(cubes_info[i + 1]);
        objects.push_back(cube_default);
    }
    // Нормализация текстур
    int scale_floor = 8;
    for (int i = 0; i < 36; ++i){
        floor.vertices[i * 8 + 6] *= (float) scale_floor;
        floor.vertices[i * 8 + 7] *= (float) scale_floor;
    }
    // Нормализация текстур
    int scale_wall = 4;
    for (int i = 0; i < 36; ++i){
        wall.vertices[i * 8 + 6] *= (float) scale_wall;
        wall.vertices[i * 8 + 7] *= (float) scale_wall;
        wall2.vertices[i * 8 + 6] *= (float) scale_wall;
        wall2.vertices[i * 8 + 7] *= (float) scale_wall;
    }
    floor.translate(glm::vec3(0.0f, -4.0f, 0.0f));
    floor.scale(glm::vec3((float) scale_floor, 0.1f, (float) scale_floor));
    objects.push_back(floor);
    floor.translate(glm::vec3(0.0f, 8.0f * (1 / 0.1f), 0.0f));
    objects.push_back(floor);
    wall.translate(glm::vec3(8.0f, 0.0f, -0.0f));
    wall.scale(glm::vec3(0.1f, (float) scale_wall , 2 * (float) scale_wall));
    objects.push_back(wall);
    wall2.translate(glm::vec3(0.0f, 0.0f, -8.0f));
    wall2.rotate(90.f,glm::vec3(0.0f, 1.0f, 0.0f));
    wall2.scale(glm::vec3(0.1f, (float) scale_wall , 2 * (float) scale_wall));
    objects.push_back(wall2);

    // Настройки skybox
    skybox_shader.use();
    unsigned int skybox_texture = load_cubetexture(R"(..\textures\skybox)", faces, false);
    skybox_shader.setInt("skybox", 9);

    // VAO скайбокса
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);


    // Создание всенаправленной карты теней
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // создаём текстуру кубической карты глубины
    unsigned int depthCubemap;
    glGenTextures(1, &depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // прикрепляем текстуру глубины в качестве буфера глубины для FBO
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /*
    // Установка текстур
    shader.use();
    shader.setInt("material.diffuse", 0);
    shader.setInt("material.specular", 1);
    shader.setInt("material.shininess;", 48);
    shader.setInt("depthMap", 10);
     */
    // Параметры света
    glm::vec3 lightPos(0.0f, 0.0f, 2.0f);
    shader.use();
    shader.setInt("depthMap", 10);

    // Точечный источник света
    shader.use();
    shader.setVec3("light.ambient",  glm::vec3(0.10f, 0.10f, 0.10f));
    shader.setVec3("light.diffuse",  glm::vec3(0.5f, 0.5f, 0.5f));
    shader.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    shader.setFloat("light.constant",  1.0f);
    shader.setFloat("light.linear",    0.07f);
    shader.setFloat("light.quadratic", 0.017f);

    // Конфигурация шейдеров
    parallax_shader.use();
    parallax_shader.setInt("diffuseMap", 4);
    parallax_shader.setInt("normalMap", 5);
    parallax_shader.setInt("depthMap", 6);

    // Игровой цикл
    while (!glfwWindowShouldClose(window))
    {
        // Обработка времени
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Обработка ввода
        processInput(window);

        // изменение позиции источника света с течением времени
        lightPos.x = sin(glfwGetTime() * 0.5) * 3.0;

        // Очистка буферов
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Создаём матрицы трансформации кубической карты глубины
        float near_plane = 1.0f;
        float far_plane = 25.0f;
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
        std::vector<glm::mat4> shadowTransforms;
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

        // Перемещение кубов
        objects[0].set_base();
        objects[0].translate(cubes_info[0] + glm::vec3(sin(glfwGetTime()) * 2, 0, 0));
        objects[0].scale(cubes_info[1]);

        objects[1].rotate(deltaTime * 100, glm::normalize(glm::vec3(1.0, 1.0, 0.0)));

        objects[2].set_base();
        objects[2].translate(cubes_info[4] + glm::vec3(0.0f, cos(glfwGetTime()) * 2, 0.0));
        objects[2].scale(cubes_info[5]);

        // Рендеринг в карту глубины
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        depthShader.use();
        for (unsigned int i = 0; i < 6; ++i)
            depthShader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
        depthShader.setFloat("far_plane", far_plane);
        depthShader.setVec3("lightPos", lightPos);
        renderScene(depthShader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Обычный рендеринг
        glViewport(0, 0, window_width, window_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Матрицы связанные с камерой
        glm::mat4 view, projection;

        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)window_width / (float)window_height, 0.1f, 100.0f);

        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        // устанавливаем uniform-переменные освещения
        shader.setVec3("light.position", lightPos);
        shader.setVec3("viewPos", camera.Pos);
        shader.setInt("shadows", shadows); // "Пробел" включает/отключает тени
        shader.setFloat("far_plane", far_plane);
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
        renderScene(shader);

        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)window_width / (float)window_height, 0.1f, 100.0f);
        parallax_shader.use();
        parallax_shader.setMat4("projection", projection);
        parallax_shader.setMat4("view", view);

        // рендеринг прямоугольника с параллакс-эффектом
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.0f, 7.0f));
        model =  glm::rotate(model, glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(2.0f));
        parallax_shader.setMat4("model", model);
        parallax_shader.setVec3("viewPos", camera.Pos);
        parallax_shader.setVec3("lightPos", lightPos);
        float heightScale = 0.1;
        parallax_shader.setFloat("heightScale", heightScale); // изменение значений кнопками Q и E
        //std::cout << heightScale << std::endl;
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, par_diffuse);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, par_normal);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, par_height);
        glDisable(GL_CULL_FACE);
        renderQuad();
        glEnable(GL_CULL_FACE);

        // Отображение источника света
        if (light_mode){
            light_cube.use();
            light.set_base();
            light.translate(lightPos);
            light.scale(glm::vec3(0.05f));
            light_cube.setMat4("projection", projection);
            light_cube.setMat4("view", view);
            light.Render(light_cube);
        }

        //Рендеринг скайбокса
        glDepthFunc(GL_LEQUAL);
        view = glm::mat4(glm::mat3(view));

        skybox_shader.use();
        skybox_shader.setMat4("view", view);
        skybox_shader.setMat4("projection", projection);

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE9);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

        // Отрисовка
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}



void renderScene(const Shader& shader) {
    /*
    // комната
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(5.0f));
    shader.setMat4("model", model);
    glDisable(GL_CULL_FACE); // обратите внимание, что здесь мы отключаем отсечение граней, т.к. рендерим "внутри" комнаты, а не "снаружи" (в обычном варианте)
    shader.setInt("reverse_normals", 1); // небольшой трюк для инвертирования нормалей при отрисовке изнутри комнаты, иначе освещение не будет работать
    renderCube();
    shader.setInt("reverse_normals", 0); // ну а теперь отключаем инвертирование нормалей
    glEnable(GL_CULL_FACE);
    */
    for (int i = 0; i < objects.size(); ++i){
        objects[i].Render(shader);
    }
}


void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.key_callback(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.key_callback(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.key_callback(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.key_callback(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !shadowsKeyPressed) {
        shadows = !shadows;
        shadowsKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !lightKeyPressed) {
        light_mode = !light_mode;
        lightKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
        shadowsKeyPressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE) {
        lightKeyPressed = false;
    }
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {

    glViewport(0, 0, width, height);
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // перевернуто, так как Y-координаты идут снизу вверх

    lastX = xpos;
    lastY = ypos;

    camera.mouse_callback(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.scroll_callback(xoffset, yoffset);
}



