#ifndef MASHGRAPH_SHADER_H
#define MASHGRAPH_SHADER_H

#define GLEW_STATIC
#include "libs/include/glew.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

string readFile(const string& fileName){
    stringstream Stream;
    ifstream File;
    File.exceptions(ifstream::badbit);
    try {
        File.open(fileName);
        Stream << File.rdbuf();
        File.close();
        return Stream.str();
    } catch (ifstream::failure e) {
        cout << "Shader read error" << endl;
    }
}

class Shader {
public:
    unsigned  int ID;
    Shader (const char* vertexPath, const char* fragmentPath){
        string vert_str = readFile(vertexPath);
        string frag_str = readFile(fragmentPath);

        //Вершинный шейдер
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        const char *src = vert_str.c_str();
        GLint success;
        GLchar infoLog[512];

        glShaderSource(vertexShader, 1, &src, nullptr);
        glCompileShader(vertexShader);
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
            cout << "VertexShader: Failed\n"<< infoLog << endl;
        }

        //Фрагментный шейдер
        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        src = frag_str.c_str();
        glShaderSource(fragmentShader, 1, &src, nullptr);
        glCompileShader(fragmentShader);
        if (!success) {
            glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
            cout << "fragmentShader: Failed\n" << infoLog<< endl;
        }

        //Шейдерная программа
        ID = glCreateProgram();

        glAttachShader(ID, vertexShader);
        glAttachShader(ID, fragmentShader);
        glLinkProgram(ID);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }
    void use() const {
        glUseProgram(ID);
    }
    void setMat4(const std::string& name, const glm::mat4& mat) const{
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void setVec3(const std::string& name, const glm::vec3& value) const {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
};


#endif //MASHGRAPH_SHADER_H
