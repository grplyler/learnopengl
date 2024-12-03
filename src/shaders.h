#ifndef SHADERS_HPP
#define SHADERS_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glad/glad.h>


std::string LoadText(std::string path) {
    std::ifstream file;
    file.open(path);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return "";
    }

    std::stringstream stream;
    stream << file.rdbuf();
    file.close();
    return stream.str();
}

void PrintShaderErrors(unsigned int shader, std::string type) {
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::" << type << "::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}

void PrintShaderProgramErrors(unsigned int program) {
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
}

int LoadShader(std::string shader_path) {
    // Where shader path containts vertex.glsl and fragment.glsl
    std::string vertexShaderPath = shader_path + "/vertex.glsl";
    std::string fragmentShaderPath = shader_path + "/fragment.glsl";
    std::string vertexShaderSource = LoadText(vertexShaderPath);
    std::string fragmentShaderSource = LoadText(fragmentShaderPath);

    // Create a vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexShaderSourceCStr = vertexShaderSource.c_str();
    glShaderSource(vertexShader, 1, &vertexShaderSourceCStr, nullptr);
    std::cout << "Compiling vertex shader: " << vertexShaderPath << std::endl;
    glCompileShader(vertexShader);

    // Check for vertex shader compile errors
    PrintShaderErrors(vertexShader, "VERTEX");

    // Create a fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentShaderSourceCStr = fragmentShaderSource.c_str();
    std::cout << "Compiling fragment shader: " << fragmentShaderPath << std::endl;
    glShaderSource(fragmentShader, 1, &fragmentShaderSourceCStr, nullptr);
    glCompileShader(fragmentShader);

    // Check for fragment shader compile errors
    PrintShaderErrors(fragmentShader, "FRAGMENT");

    // Link the shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    std::cout << "Linking shader program: " << vertexShaderPath << " and " << fragmentShaderPath << std::endl;
    glLinkProgram(shaderProgram);

    // Check for linking errors
    PrintShaderProgramErrors(shaderProgram);

    // Delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Return the shader program
    return shaderProgram;
}

#endif