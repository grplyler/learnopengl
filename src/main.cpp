#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "shaders.h"

struct Mesh {
    float *vertices;
    unsigned int *indices;
    unsigned int n_vertices;
    unsigned int n_indices;
    unsigned int VBO, VAO, EBO;
    bool gpu_loaded;
};

void MeshLoad(Mesh *mesh);
void MeshFree(Mesh *mesh);

Mesh MakeMeshGrid(int rows, int cols, bool load = false) {
    Mesh mesh;
    mesh.n_vertices = rows * cols;
    mesh.n_indices = (rows - 1) * (cols - 1) * 6;
    mesh.vertices = new float[mesh.n_vertices * 3];
    mesh.indices = new unsigned int[mesh.n_indices];

    float width = 2.0;
    float height = 2.0;
    
    float dx = width / (cols - 1);
    float dy = height / (rows - 1);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            mesh.vertices[3 * (i * cols + j)] = j * dx - width / 2;
            mesh.vertices[3 * (i * cols + j) + 1] = i * dy - height / 2;
            mesh.vertices[3 * (i * cols + j) + 2] = 0;
        }
    }

    int index = 0;
    for (int i = 0; i < rows - 1; i++) {
        for (int j = 0; j < cols - 1; j++) {
            // Top-left triangle
            mesh.indices[index++] = i * cols + j;
            mesh.indices[index++] = i * cols + (j + 1);
            mesh.indices[index++] = (i + 1) * cols + j;

            // Bottom-right triangle
            mesh.indices[index++] = i * cols + (j + 1);
            mesh.indices[index++] = (i + 1) * cols + (j + 1);
            mesh.indices[index++] = (i + 1) * cols + j;
        }
    }

    if (load) {
        MeshLoad(&mesh);
    }
    return mesh;
}

void MeshLoad(Mesh *mesh) {
    if (mesh->gpu_loaded) {
        std::cerr << "Mesh already loaded on GPU" << std::endl;
        return;
    }

    glGenVertexArrays(1, &mesh->VAO);
    glGenBuffers(1, &mesh->VBO);
    glGenBuffers(1, &mesh->EBO);

    glBindVertexArray(mesh->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh->n_vertices * 3 * sizeof(float), mesh->vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->n_indices * sizeof(unsigned int), mesh->indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    mesh->gpu_loaded = true;
}

void MeshFree(Mesh *mesh) {
    // Get rid of the buffers
    glDeleteVertexArrays(1, &mesh->VAO);
    glDeleteBuffers(1, &mesh->VBO);
    glDeleteBuffers(1, &mesh->EBO);

    // Get rid of the data
    delete[] mesh->vertices;
    delete[] mesh->indices;

    mesh->gpu_loaded = false;

}

void DrawMesh(Mesh mesh) {
    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, mesh.n_indices, GL_UNSIGNED_INT, 0);
}

void DrawMeshEx(Mesh mesh, unsigned int mode) {
    if (!mesh.gpu_loaded) {
        std::cerr << "Mesh not loaded on GPU" << std::endl;
        return;
    }
    glBindVertexArray(mesh.VAO);
    glDrawElements(mode, mesh.n_indices, GL_UNSIGNED_INT, 0);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}  

void processInput(GLFWwindow *window) 
{
    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Starter", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }



    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Set the viewport
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Configure OpenGL
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    // My Stuff
    int shaderProgram = LoadShader("/Users/ryan/code/learnopengl/src/shaders/default");

    float vertices[] = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    }; 

    // Setup vertex data
    Mesh mesh = MakeMeshGrid(10, 10, true);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // input
        processInput(window);

        // rendering commands
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(shaderProgram);
        DrawMeshEx(mesh, GL_TRIANGLES);

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    MeshFree(&mesh);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
