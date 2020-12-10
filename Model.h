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
    int size;
    Material material;
    unsigned int VAO;
    glm::mat4 model;

    TriangleMesh(float* vertices, int size, unsigned int diffuse_map, unsigned int spec_map, float shininess = 32.0f) {
        this->vertices = vertices;
        this->material.diffuse_map = diffuse_map;
        this->material.spec_map = spec_map;
        this->material.shininess = shininess;
        this->size = size;
        model = glm::mat4(1.0f);
        setupMesh();
    }
    ~TriangleMesh(){
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
    void translate(glm::vec3 pos){
        model = glm::translate(model, pos);
    }
    void scale(glm::vec3 pos){
        model = glm::scale(model, glm::vec3(0.2f));
    }
    void Render(Shader& shader) {
        // Связываем соответствующие текстуры
        shader.use();
        shader.setFloat("material.shininess", material.shininess);
        shader.setInt("material.diffuse", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, material.diffuse_map);

        shader.setInt("material.specular", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, material.spec_map);
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

        glBindVertexArray(VAO);

        // Загружаем данные в вершинный буфер
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
        /*
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        */
        // Атрибут с координатами
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*) 0);
        glEnableVertexAttribArray(0);
        // Тексутры
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*) (3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        // Нормали
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*) (5 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
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

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

class Model
{
public:
    // данные модели
    vector<Texture> textures_loaded;	// (оптимизация) сохраняем все загруженные текстуры, чтобы убедиться, что они не загружены более одного раза
    vector<Mesh>    meshes;
    string directory;
    bool gammaCorrection;

    // конструктор, в качестве аргумента использует пусть до 3d-модели
    Model(string const &path, bool gamma = false) : gammaCorrection(gamma)
    {
        loadModel(path);
    }

    // отрисовывает модель, а значит и все её меши
    void Draw(Shader shader)
    {
        for(unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

private:
    // загружает модель с помощью Assimp и сохраняет полученные меши в векторе meshes.
    void loadModel(string const &path)
    {
        // чтение файла с помощью ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // проверка на ошибки
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // если НЕ 0
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }
        // получение пути к файлу
        directory = path.substr(0, path.find_last_of('/'));

        // рекурсивная обработка корневого узла ASSIMP
        processNode(scene->mRootNode, scene);
    }

    // рекурсивная обработка узла. Обрабатывает каждый отдельный меш, расположенный в узле, и повторяет этот процесс для своих дочерних углов (если таковы вообще имеются).
    void processNode(aiNode *node, const aiScene *scene)
    {
        // обрабатываем каждый меш текущего узла
        for(unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // узел содержит только индексы объектов в сцене
            // сцена же содержит все данные; узел - это лишь способ организации данных
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        // после того, как мы обработали все меши (если таковы имелись), мы начинаем рекурсивно обрабатывать каждый из дочерних узлов
        for(unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }

    }

    Mesh processMesh(aiMesh *mesh, const aiScene *scene)
    {
        // данные для заполнения
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        // цикл по всем вершинам меша
        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; // мы объявляем промежуточный вектор, т.к. assimp использует свой собственный векторный класс, который не преобразуется напрямую в тип glm::vec3, поэтому сначала мы передаем данные в этот промежуточный вектор типа glm::vec3
            // координаты
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // нормали
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
            // текстурные координаты
            if(mesh->mTextureCoords[0]) // если меш содержит текстурные координаты
            {
                glm::vec2 vec;
                // вершина может содержать до 8 различных текстурных координат. Мы предполагаем, что что мы не будем использовать модели,
                // в которых вершина может содержать несколько текстурных координат, поэтому мы всегда берем первый набор (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            // касательный вектор
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
            // вектор бинормали
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
            vertices.push_back(vertex);
        }
        // теперь проходимся по каждой грани меша (грань - это треугольник меша) и извлекаем соответствующие индексы вершин
        for(unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // получаем все индексы граней и сохраняем их в векторе indices
            for(unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        // обрабатываем материалы
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        // мы вводим соглашение об именах сэмплеров в шейдерах. Каждая диффузная текстура будет называться 'texture_diffuseN',
        // где N - порядковый номер от 1 до MAX_SAMPLER_NUMBER.
        // Тоже самое относится и к другим текстурам:
        // диффузная: texture_diffuseN
        // отражения: texture_specularN
        // нормали: texture_normalN

        // 1. диффузные карты
        vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. карты отражения
        vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. карты нормалей
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. карты высот
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        // возвращаем mesh-объект, созданный на основе полученных данных
        return Mesh(vertices, indices, textures);
    }

    // проверяем все текстуры материалов заданного типа и загружам текстуры, если они еще не были загружены.
    // необходимая информация возвращается в виде struct Texture.
    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
    {
        vector<Texture> textures;
        for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            // проверяем, не была ли текстура загружена ранее, и если - да, то пропускаем загрузку новой текстуры и переходим к следующей итерации
            bool skip = false;
            for(unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true; // текстура с тем же путем к файлу уже загружена, переходим к следующей. (оптимизация)
                    break;
                }
            }
            if(!skip)
            {   // если текстура еще не была загружена - загружаем её
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);  // сохраняем её в массиве с уже загруженными текстурами, тем самым гарантировав, что у нас не появятся без необходимости дубликаты текстур
            }
        }
        return textures;
    }
};


unsigned int TextureFromFile(const char *path, const string &directory, bool gamma)
{
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = SOIL_load_image(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        SOIL_free_image_data(data);
    }
    else
        {
        std::cout << "Texture failed to load at path: " << path << std::endl;
            SOIL_free_image_data(data);
    }

    return textureID;
}
#endif //MASHGRAPH_MODEL_H
