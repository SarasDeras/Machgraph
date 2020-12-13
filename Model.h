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

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

struct Material {
    unsigned int diffuse_map;
    unsigned int spec_map;
    float shininess;
};


struct Vertex {
    glm::vec3 Position;
    // Нормаль
    glm::vec3 Normal;
    // Текстурные координаты
    glm::vec2 TexCoords;
    // Касательный вектор
    glm::vec3 Tangent;
    // Вектор бинормали (вектор, перпендикулярный касательному вектору и вектору нормали)
    glm::vec3 Bitangent;
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};

class TriangleMesh{
    public:
    float* vertices;
    unsigned int size;
    Material material;
    unsigned int VAO;
    glm::mat4 model;

    TriangleMesh(float* vertices, unsigned int size, unsigned int diffuse_map, unsigned int spec_map, float shininess = 64.0f) {
        this->vertices = vertices;
        this->material.diffuse_map = diffuse_map;
        this->material.spec_map = spec_map;
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
    void Render(const Shader& shader, bool maps=true) const {
        shader.use();
        if (maps){
            // Связываем соответствующие текстуры
            shader.setFloat("material.shininess", material.shininess);
            shader.setInt("material.diffuse", 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, material.diffuse_map);

            shader.setInt("material.specular", 1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, material.spec_map);
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

class Mesh {
public:
    // Данные mesh-а
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    unsigned int VAO;

    // Конструктор
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
    {
        this->vertices = std::move(vertices);
        this->indices = std::move(indices);
        this->textures = textures;

        // Теперь, когда у нас есть все необходимые данные, устанавливаем вершинные буферы и указатели атрибутов
        setupMesh();
    }


    // Рендеринг mesh-а
    void Draw(Shader& shader)
    {
        // Связываем соответствующие текстуры
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // перед связыванием активируем нужный текстурный юнит
            // Получаем номер текстуры (номер N в diffuse_textureN)
            string number;
            string name = textures[i].type;
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++); // конвертируем unsigned int в строку
            else if (name == "texture_normal")
                number = std::to_string(normalNr++); // конвертируем unsigned int в строку
            else if (name == "texture_height")
                number = std::to_string(heightNr++); // конвертируем unsigned int в строку

            // Теперь устанавливаем сэмплер на нужный текстурный юнит
            glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
            // и связываем текстуру
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        // Отрисовываем mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Считается хорошей практикой возвращать значения переменных к их первоначальным значениям
        glActiveTexture(GL_TEXTURE0);
    }

private:
    // Данные для рендеринга
    unsigned int VBO, EBO;

    // Инициализируем все буферные объекты/массивы
    void setupMesh()
    {
        // Создаем буферные объекты/массивы
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        // Загружаем данные в вершинный буфер
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        // Самое замечательное в структурах то, что расположение в памяти их внутренних переменных является последовательным.
        // Смысл данного трюка в том, что мы можем просто передать указатель на структуру, и она прекрасно преобразуется в массив данных с элементами типа glm::vec3 (или glm::vec2), который затем будет преобразован в массив данных float, ну а в конце – в байтовый массив
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // Устанавливаем указатели вершинных атрибутов

        // Координаты вершин
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        // Нормали вершин
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

        // Текстурные координаты вершин
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

        // Касательный вектор вершины
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

        // Вектор бинормали вершины
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

        glBindVertexArray(0);
    }
};
#endif //MASHGRAPH_MODEL_H
