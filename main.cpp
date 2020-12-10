#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#define GLEW_STATIC
#include "libs/include/glew.h"
#include "libs/include/glfw3.h"
#include "libs/include/SOIL.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



using namespace std;
//Параметры окна
int window_width = 800, window_height = 600;

// Обработка мыши
float lastX = (float) window_width / 2.0f, lastY = (float) window_height / 2.0f;
bool firstMouse = true;
// Отсчёт промежутков вермени между отрисовской
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Камера
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

// Освещение
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

//Обработчики событий
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.key_callback(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.key_callback(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.key_callback(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.key_callback(RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.mouse_callback(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.scroll_callback(xoffset, yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // убеждаемся, что окно просмотра соответствует новым размерам окна
    // обратите внимание, что ширина и высота будут значительно больше, чем указано на Retina-дисплеях
    glViewport(0, 0, width, height);
}

unsigned int load_texture(string path){
    unsigned int texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width_t, height_t;
    unsigned char* image = SOIL_load_image(path.c_str(), &width_t, &height_t, 0, SOIL_LOAD_RGB);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_t, height_t, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
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
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    //Обработка глубины
    glEnable(GL_DEPTH_TEST);

    //Шейдеры
    Shader obj_shader(R"(..\shaders\default.vert)", R"(..\shaders\default.frag)");
    Shader light_cube(R"(..\shaders\light.vert)", R"(..\shaders\light.frag)");

    //Примитивы
    GLfloat vertices[] = {
            // Координаты         // Текстуры  // Нормали
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,

            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, -1.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, -1.0f,  0.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f
    };


    // Загрузка текстур
    unsigned int cube_diffuse = load_texture(R"(..\textures\box.png)");
    unsigned int cube_spec = load_texture(R"(..\textures\box_specular.png)");

    //Созданние mesheй
    TriangleMesh cube(vertices, sizeof(vertices), cube_diffuse, cube_spec);
    TriangleMesh light(vertices, sizeof(vertices), cube_diffuse, cube_spec);
    light.translate(lightPos);
    light.scale(glm::vec3(0.2f));

    // Параметры света
    // Точечный источник света
    obj_shader.use();
    obj_shader.setVec3("light.ambient",  glm::vec3(0.2f, 0.2f, 0.2f));
    obj_shader.setVec3("light.diffuse",  glm::vec3(0.75f, 0.75f, 0.75f));
    obj_shader.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    obj_shader.setVec3("light.position", lightPos);
    obj_shader.setFloat("light.constant",  1.0f);
    obj_shader.setFloat("light.linear",    0.09f);
    obj_shader.setFloat("light.quadratic", 0.032f);

    // Игровой цикл
    while(!glfwWindowShouldClose(window)) {
        // Обработка времени
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Cобытия
        glfwPollEvents();

        //Очистка буферов
        glClearColor(0.1f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // Матрицы связанные с камерой
        glm::mat4 view, projection;
        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), (float) window_width / (float) window_height, 0.1f, 100.0f);

        obj_shader.use();
        obj_shader.setMat4("view", view);
        obj_shader.setMat4("projection", projection);
        obj_shader.setVec3("viewPos", camera.Pos);

        light_cube.use();
        light_cube.setMat4("projection", projection);
        light_cube.setMat4("view", view);



        // Рендеринг объектов
        cube.Render(obj_shader);
        light.Render(light_cube);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}



