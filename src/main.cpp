#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <unordered_map>
#include <tuple>
#include "shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct Mesh {
    float *vertices;
    float *colors;
    float *texcoords;
    unsigned int *indices;
    unsigned int n_vertices;
    unsigned int n_indices;
    unsigned int VBO, VAO, EBO, CBO, TBO;
    bool has_vertex_colors = false;
    bool has_texture_coords = false;
    bool gpu_loaded = false;
};

void MeshLoad(Mesh *mesh);
void MeshFree(Mesh *mesh);

Mesh MakeTriangle(bool load_to_gpu = false) {
    Mesh mesh;
    mesh.n_vertices = 3;
    mesh.n_indices = 3;
    mesh.vertices = new float[mesh.n_vertices * 3];
    mesh.indices = new unsigned int[mesh.n_indices];

    mesh.vertices[0] = 0.0f;
    mesh.vertices[1] = 0.5f;
    mesh.vertices[2] = 0.0f;

    mesh.vertices[3] = 0.5f;
    mesh.vertices[4] = -0.5f;
    mesh.vertices[5] = 0.0f;

    mesh.vertices[6] = -0.5f;
    mesh.vertices[7] = -0.5f;
    mesh.vertices[8] = 0.0f;

    mesh.indices[0] = 0;
    mesh.indices[1] = 1;
    mesh.indices[2] = 2;

    if (load_to_gpu) {
        MeshLoad(&mesh);
    }
    return mesh;
}

Mesh MakeTriangleColored(bool load_to_gpu = false) {
    Mesh mesh;
    mesh.n_vertices = 3;
    mesh.n_indices = 3;
    mesh.vertices = new float[mesh.n_vertices * 3];
    mesh.colors = new float[mesh.n_vertices * 3]; // Allocate memory for colors
    mesh.indices = new unsigned int[mesh.n_indices];

    // Vertex positions
    mesh.vertices[0] = 0.0f; mesh.vertices[1] = 0.5f; mesh.vertices[2] = 0.0f;
    mesh.vertices[3] = 0.5f; mesh.vertices[4] = -0.5f; mesh.vertices[5] = 0.0f;
    mesh.vertices[6] = -0.5f; mesh.vertices[7] = -0.5f; mesh.vertices[8] = 0.0f;

    // Vertex colors
    mesh.colors[0] = 1.0f; mesh.colors[1] = 0.0f; mesh.colors[2] = 0.0f; // Red
    mesh.colors[3] = 0.0f; mesh.colors[4] = 1.0f; mesh.colors[5] = 0.0f; // Green
    mesh.colors[6] = 0.0f; mesh.colors[7] = 0.0f; mesh.colors[8] = 1.0f; // Blue

    // Indices
    mesh.indices[0] = 0;
    mesh.indices[1] = 1;
    mesh.indices[2] = 2;

    mesh.has_vertex_colors = true;

    if (load_to_gpu) {
        MeshLoad(&mesh);
    }
    return mesh;
}

Mesh MakeTriangleTextured(bool load_to_gpu = false) {
    Mesh mesh;
    mesh.n_vertices = 3;
    mesh.n_indices = 3;
    mesh.vertices = new float[mesh.n_vertices * 3];
    mesh.texcoords = new float[mesh.n_vertices * 2]; // Allocate memory for texture coordinates
    mesh.indices = new unsigned int[mesh.n_indices];

    // Vertex positions
    mesh.vertices[0] = 0.0f; mesh.vertices[1] = 0.5f; mesh.vertices[2] = 0.0f;
    mesh.vertices[3] = 0.5f; mesh.vertices[4] = -0.5f; mesh.vertices[5] = 0.0f;
    mesh.vertices[6] = -0.5f; mesh.vertices[7] = -0.5f; mesh.vertices[8] = 0.0f;

    // Texture coordinates
    mesh.texcoords[0] = 0.5f; mesh.texcoords[1] = 1.0f;
    mesh.texcoords[2] = 1.0f; mesh.texcoords[3] = 0.0f;
    mesh.texcoords[4] = 0.0f; mesh.texcoords[5] = 0.0f;

    // Indices
    mesh.indices[0] = 0;
    mesh.indices[1] = 1;
    mesh.indices[2] = 2;

    mesh.has_texture_coords = true;

    if (load_to_gpu) {
        MeshLoad(&mesh);
    }
    return mesh;
}

Mesh MakePlaneTextured(bool load_to_gpu = false) {
    Mesh mesh;
    mesh.n_vertices = 4;
    mesh.n_indices = 6;
    mesh.vertices = new float[mesh.n_vertices * 3];
    mesh.colors = new float[mesh.n_vertices * 3]; // Allocate memory for colors
    mesh.texcoords = new float[mesh.n_vertices * 2]; // Allocate memory for texture coordinates
    mesh.indices = new unsigned int[mesh.n_indices];

    // Vertex positions
    mesh.vertices[0] = -0.5f; mesh.vertices[1] = -0.5f; mesh.vertices[2] = 0.0f;
    mesh.vertices[3] = 0.5f; mesh.vertices[4] = -0.5f; mesh.vertices[5] = 0.0f;
    mesh.vertices[6] = 0.5f; mesh.vertices[7] = 0.5f; mesh.vertices[8] = 0.0f;
    mesh.vertices[9] = -0.5f; mesh.vertices[10] = 0.5f; mesh.vertices[11] = 0.0f;

    // Vertex Colors
    mesh.colors[0] = 1.0f; mesh.colors[1] = 0.0f; mesh.colors[2] = 0.0f; // Red
    mesh.colors[3] = 0.0f; mesh.colors[4] = 1.0f; mesh.colors[5] = 0.0f; // Green
    mesh.colors[6] = 0.0f; mesh.colors[7] = 0.0f; mesh.colors[8] = 1.0f; // Blue
    mesh.colors[9] = 1.0f; mesh.colors[10] = 1.0f; mesh.colors[11] = 0.0f; // Yellow

    // Texture coordinates
    mesh.texcoords[0] = 0.0f; mesh.texcoords[1] = 0.0f;
    mesh.texcoords[2] = 1.0f; mesh.texcoords[3] = 0.0f;
    mesh.texcoords[4] = 1.0f; mesh.texcoords[5] = 1.0f;
    mesh.texcoords[6] = 0.0f; mesh.texcoords[7] = 1.0f;

    // Indices
    mesh.indices[0] = 0;
    mesh.indices[1] = 1;
    mesh.indices[2] = 2;
    mesh.indices[3] = 0;
    mesh.indices[4] = 2;
    mesh.indices[5] = 3;

    mesh.has_texture_coords = true;
    mesh.has_vertex_colors = true;

    if (load_to_gpu) {
        MeshLoad(&mesh);
    }
    return mesh;
}


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

void MeshLoadInterleaved(Mesh *mesh) {
    std::cout << "Loading mesh to GPU" << std::endl;
    if (mesh->gpu_loaded) {
        std::cerr << "Mesh already loaded on GPU" << std::endl;
        return;
    }

    glGenVertexArrays(1, &mesh->VAO);
    glGenBuffers(1, &mesh->VBO);
    glGenBuffers(1, &mesh->EBO);

    glBindVertexArray(mesh->VAO);

    // Position buffer
    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh->n_vertices * 5 * sizeof(float), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, mesh->n_vertices * 3 * sizeof(float), mesh->vertices);
    glBufferSubData(GL_ARRAY_BUFFER, mesh->n_vertices * 3 * sizeof(float), mesh->n_vertices * 2 * sizeof(float), mesh->texcoords);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture Coords buffer
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->n_indices * sizeof(unsigned int), mesh->indices, GL_STATIC_DRAW);

    mesh->gpu_loaded = true;
}

void MeshLoad(Mesh* mesh) {
    std::cout << "Loading mesh to GPU" << std::endl;
    if (mesh->gpu_loaded) {
        std::cerr << "Mesh already loaded on GPU" << std::endl;
        return;
    }

    glGenVertexArrays(1, &mesh->VAO);
    glGenBuffers(1, &mesh->VBO);
    glGenBuffers(1, &mesh->EBO);

    glBindVertexArray(mesh->VAO);

    // Position buffer
    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh->n_vertices * 3 * sizeof(float), mesh->vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color buffer
    if (mesh->has_vertex_colors) {
        glGenBuffers(1, &mesh->CBO);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->CBO);
        glBufferData(GL_ARRAY_BUFFER, mesh->n_vertices * 3 * sizeof(float), mesh->colors, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
    }

    // Texture Coords buffer
    if (mesh->has_texture_coords) {
        glGenBuffers(1, &mesh->TBO);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->TBO);
        glBufferData(GL_ARRAY_BUFFER, mesh->n_vertices * 2 * sizeof(float), mesh->texcoords, GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(2);
    }

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->n_indices * sizeof(unsigned int), mesh->indices, GL_STATIC_DRAW);

    mesh->gpu_loaded = true;
}



void MeshFree(Mesh* mesh) {
    if (mesh->gpu_loaded) {
        glDeleteVertexArrays(1, &mesh->VAO);
        glDeleteBuffers(1, &mesh->VBO);
        glDeleteBuffers(1, &mesh->EBO);
        if (mesh->has_vertex_colors) {
            glDeleteBuffers(1, &mesh->CBO);
        }
    }

    delete[] mesh->vertices;
    delete[] mesh->colors;
    delete[] mesh->indices;

    mesh->gpu_loaded = false;
}

unsigned int LoadTexture(const char *path) {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering parameters
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); 
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format;
        if (nrChannels == 1) {
            format = GL_RED;
        } else if (nrChannels == 3) {
            std::cout << "RGB" << std::endl;
            format = GL_RGB;
        } else if (nrChannels == 4) {
            std::cout << "RGBA" << std::endl;
            format = GL_RGBA;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Configure Mipmaps
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 3);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 3);
        
    } else {
        std::cerr << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    return texture;
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

void SetUniform4f(int shaderProgram, const char* name, float v0, float v1, float v2, float v3) {
    int location = glGetUniformLocation(shaderProgram, name);
    glUniform4f(location, v0, v1, v2, v3);
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
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // My Stuff
    Shader shader("/Users/ryan/code/learnopengl/src/shaders/default/vertex.glsl", "/Users/ryan/code/learnopengl/src/shaders/default/fragment.glsl");

    // Load Texture
    unsigned int texture1 = LoadTexture("/Users/ryan/code/learnopengl/assets/textures/container.jpg");
    unsigned int texture2 = LoadTexture("/Users/ryan/code/learnopengl/assets/textures/awesomeface.png");

    // Setup vertex data
    // Mesh mesh = MakeMeshGrid(10, 10, true);
    // Mesh mesh = MakeTriangle(true);
    // Mesh mesh = MakeTriangleTextured(true);
    Mesh mesh = MakePlaneTextured(false);
    MeshLoad(&mesh);

    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

    // State
    float fps = 0.0f;
    double lastTime = glfwGetTime();
    int nbFrames = 0;

    glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));
    vec = trans * vec;
    std::cout << vec.x << vec.y << vec.z << std::endl;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // input
        processInput(window);

        // rendering commands
        glClear(GL_COLOR_BUFFER_BIT);
        shader.use();
        shader.setInt("texture1", 0);
        shader.setInt("texture2", 1);

        // float timeValue = glfwGetTime();
        // float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
        // SetUniform4f(shaderProgram, "ourColor", 0.0f, greenValue, 0.0f, 1.0f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
        trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

        unsigned int transformLoc = glGetUniformLocation(shader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

        DrawMeshEx(mesh, GL_TRIANGLES);

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Calculate FPS
        double currentTime = glfwGetTime();
        nbFrames++;
        if (currentTime - lastTime >= 1.0) {
            fps = 1000.0 / double(nbFrames);
            nbFrames = 0;
            lastTime += 1.0;

            std::cout << "FPS: " << fps << std::endl;   
        }

    }

    // Cleanup
    MeshFree(&mesh);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
