#ifndef MASHGRAPH_MODEL_H
#define MASHGRAPH_MODEL_H

#include <string>
#include <utility>
#include <vector>
#include "Shader.h"
#define GLEW_STATIC
#include "libs/include/glew.h"
#include "libs/include/glfw3.h"
#include "libs/include/SOIL.h"
#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

struct Material {
    unsigned int diffuse_map;
    unsigned int spec_map;
    unsigned int normal_map;
    bool specular_flag = false;
    bool normal_map_flag = false;
    float shininess;
};

class TriangleMesh{
    public:
    float* vertices;
    unsigned int size;
    Material material;
    unsigned int VAO;
    glm::mat4 model;


    TriangleMesh(float* vertices, unsigned int size, unsigned int diffuse_map, unsigned int spec_map = 0,
            unsigned int normal_map = 0, float shininess = 64.0f) {
        this->vertices = vertices;
        this->material.diffuse_map = diffuse_map;
        if (spec_map > 0){
            this->material.spec_map = spec_map;
            material.specular_flag = true;
        }
        if (normal_map > 0){
            this->material.normal_map = normal_map;
            material.normal_map_flag = true;
        }
        this->material.shininess = shininess;
        this->size = size;
        model = glm::mat4(1.0f);
        setupMesh();
    }
    void translate(glm::vec3 pos){
        model = glm::translate(model, pos);
    }
    void set(glm::mat4 m){
        model = m;
    }
    void scale(glm::vec3 scale){
        model = glm::scale(model, scale);
    }
    void set_base(){
        model = glm::mat4(1.0f);
    }
    void rotate(float angle, glm::vec3 r_vector){
        model = glm::rotate(model, glm::radians(angle), r_vector);
    }
    void Render(const Shader& shader) const {
        shader.use();
        // Связываем соответствующие текстуры
        shader.setFloat("material.shininess", material.shininess);
        shader.setInt("material.diffuse", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, material.diffuse_map);

        shader.setInt("material.specular", 1);
        shader.setInt("material.specular_flag", material.specular_flag);
        if (material.specular_flag){
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, material.spec_map);
        }
        shader.setInt("material.normal_map_flag", material.normal_map_flag);
        if (material.normal_map_flag){
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, material.normal_map);
        }

        // Передаём матрицу модели
        shader.setMat4("model", model);
        // Отрисовываем mesh
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, size);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
    }
private:
    // Данные для рендеринга
    unsigned int VBO;

    // Инициализируем все буферные объекты/массивы
    void setupMesh() {
        // Создаем буферные объекты/массивы
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        // Загружаем данные в вершинный буфер
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
        /*
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        */
        glBindVertexArray(VAO);
        // Атрибут с координатами
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        // Нормали
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        // Тексутры
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
};

#endif //MASHGRAPH_MODEL_H
