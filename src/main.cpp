#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <unordered_map>
#include <tuple>
#include <thread> // For std::this_thread::sleep_for
#include <chrono> // For std::chrono::duration
#include "shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct Mesh
{
    float *vertices;
    float *colors;
    float *texcoords;
    unsigned int *indices;
    unsigned int n_vertices;
    unsigned int n_indices;
    unsigned int VBO, VAO, EBO, CBO, TBO;
    bool is_interleaved = false;
    bool has_vertex_colors = false;
    bool has_texture_coords = false;
    bool gpu_loaded = false;
};

void MeshLoad(Mesh *mesh);
void MeshLoadInterleaved(Mesh *mesh);
void MeshFree(Mesh *mesh);

Mesh MakeTriangle(bool load_to_gpu = false)
{
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

    if (load_to_gpu)
    {
        MeshLoad(&mesh);
    }
    return mesh;
}

Mesh MakeTriangleColored(bool load_to_gpu = false)
{
    Mesh mesh;
    mesh.n_vertices = 3;
    mesh.n_indices = 3;
    mesh.vertices = new float[mesh.n_vertices * 3];
    mesh.colors = new float[mesh.n_vertices * 3]; // Allocate memory for colors
    mesh.indices = new unsigned int[mesh.n_indices];

    // Vertex positions
    mesh.vertices[0] = 0.0f;
    mesh.vertices[1] = 0.5f;
    mesh.vertices[2] = 0.0f;
    mesh.vertices[3] = 0.5f;
    mesh.vertices[4] = -0.5f;
    mesh.vertices[5] = 0.0f;
    mesh.vertices[6] = -0.5f;
    mesh.vertices[7] = -0.5f;
    mesh.vertices[8] = 0.0f;

    // Vertex colors
    mesh.colors[0] = 1.0f;
    mesh.colors[1] = 0.0f;
    mesh.colors[2] = 0.0f; // Red
    mesh.colors[3] = 0.0f;
    mesh.colors[4] = 1.0f;
    mesh.colors[5] = 0.0f; // Green
    mesh.colors[6] = 0.0f;
    mesh.colors[7] = 0.0f;
    mesh.colors[8] = 1.0f; // Blue

    // Indices
    mesh.indices[0] = 0;
    mesh.indices[1] = 1;
    mesh.indices[2] = 2;

    mesh.has_vertex_colors = true;

    if (load_to_gpu)
    {
        MeshLoad(&mesh);
    }
    return mesh;
}

Mesh MakeTriangleTextured(bool load_to_gpu = false)
{
    Mesh mesh;
    mesh.n_vertices = 3;
    mesh.n_indices = 3;
    mesh.vertices = new float[mesh.n_vertices * 3];
    mesh.texcoords = new float[mesh.n_vertices * 2]; // Allocate memory for texture coordinates
    mesh.indices = new unsigned int[mesh.n_indices];

    // Vertex positions
    mesh.vertices[0] = 0.0f;
    mesh.vertices[1] = 0.5f;
    mesh.vertices[2] = 0.0f;
    mesh.vertices[3] = 0.5f;
    mesh.vertices[4] = -0.5f;
    mesh.vertices[5] = 0.0f;
    mesh.vertices[6] = -0.5f;
    mesh.vertices[7] = -0.5f;
    mesh.vertices[8] = 0.0f;

    // Texture coordinates
    mesh.texcoords[0] = 0.5f;
    mesh.texcoords[1] = 1.0f;
    mesh.texcoords[2] = 1.0f;
    mesh.texcoords[3] = 0.0f;
    mesh.texcoords[4] = 0.0f;
    mesh.texcoords[5] = 0.0f;

    // Indices
    mesh.indices[0] = 0;
    mesh.indices[1] = 1;
    mesh.indices[2] = 2;

    mesh.has_texture_coords = true;

    if (load_to_gpu)
    {
        MeshLoad(&mesh);
    }
    return mesh;
}

Mesh MakePlaneTextured(bool load_to_gpu = false)
{
    Mesh mesh;
    mesh.n_vertices = 4;
    mesh.n_indices = 6;
    mesh.vertices = new float[mesh.n_vertices * 3];
    mesh.colors = new float[mesh.n_vertices * 3];    // Allocate memory for colors
    mesh.texcoords = new float[mesh.n_vertices * 2]; // Allocate memory for texture coordinates
    mesh.indices = new unsigned int[mesh.n_indices];

    // Vertex positions
    mesh.vertices[0] = -0.5f;
    mesh.vertices[1] = -0.5f;
    mesh.vertices[2] = 0.0f;
    mesh.vertices[3] = 0.5f;
    mesh.vertices[4] = -0.5f;
    mesh.vertices[5] = 0.0f;
    mesh.vertices[6] = 0.5f;
    mesh.vertices[7] = 0.5f;
    mesh.vertices[8] = 0.0f;
    mesh.vertices[9] = -0.5f;
    mesh.vertices[10] = 0.5f;
    mesh.vertices[11] = 0.0f;

    // Vertex Colors
    mesh.colors[0] = 1.0f;
    mesh.colors[1] = 0.0f;
    mesh.colors[2] = 0.0f; // Red
    mesh.colors[3] = 0.0f;
    mesh.colors[4] = 1.0f;
    mesh.colors[5] = 0.0f; // Green
    mesh.colors[6] = 0.0f;
    mesh.colors[7] = 0.0f;
    mesh.colors[8] = 1.0f; // Blue
    mesh.colors[9] = 1.0f;
    mesh.colors[10] = 1.0f;
    mesh.colors[11] = 0.0f; // Yellow

    // Texture coordinates
    mesh.texcoords[0] = 0.0f;
    mesh.texcoords[1] = 0.0f;
    mesh.texcoords[2] = 1.0f;
    mesh.texcoords[3] = 0.0f;
    mesh.texcoords[4] = 1.0f;
    mesh.texcoords[5] = 1.0f;
    mesh.texcoords[6] = 0.0f;
    mesh.texcoords[7] = 1.0f;

    // Indices
    mesh.indices[0] = 0;
    mesh.indices[1] = 1;
    mesh.indices[2] = 2;
    mesh.indices[3] = 0;
    mesh.indices[4] = 2;
    mesh.indices[5] = 3;

    mesh.has_texture_coords = true;
    mesh.has_vertex_colors = true;

    if (load_to_gpu)
    {
        MeshLoad(&mesh);
    }
    return mesh;
}

Mesh MakeCubeTextured(bool load_to_gpu = false)
{
    Mesh mesh;
    mesh.is_interleaved = true;
    mesh.n_vertices = 36;                           // 6 faces * 2 triangles/face * 3 vertices/triangle
    mesh.vertices = new float[mesh.n_vertices * 5]; // 3 position + 2 texture coords

    // Cube vertex data with texture coordinates
    float vertices[] = {
        // Positions          // Texture Coords
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f};

    // Copy vertex data to mesh
    for (int i = 0; i < mesh.n_vertices * 5; i++)
    {
        mesh.vertices[i] = vertices[i];
    }

    mesh.has_vertex_colors = true;
    mesh.has_texture_coords = true;

    if (load_to_gpu)
    {
        MeshLoadInterleaved(&mesh); // Load to GPU
    }

    return mesh;
}

Mesh MakeMeshGrid(int rows, int cols, bool load = false)
{
    Mesh mesh;
    mesh.n_vertices = rows * cols;
    mesh.n_indices = (rows - 1) * (cols - 1) * 6;
    mesh.vertices = new float[mesh.n_vertices * 3];
    mesh.indices = new unsigned int[mesh.n_indices];

    float width = 2.0;
    float height = 2.0;

    float dx = width / (cols - 1);
    float dy = height / (rows - 1);

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            mesh.vertices[3 * (i * cols + j)] = j * dx - width / 2;
            mesh.vertices[3 * (i * cols + j) + 1] = i * dy - height / 2;
            mesh.vertices[3 * (i * cols + j) + 2] = 0;
        }
    }

    int index = 0;
    for (int i = 0; i < rows - 1; i++)
    {
        for (int j = 0; j < cols - 1; j++)
        {
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

    if (load)
    {
        MeshLoad(&mesh);
    }
    return mesh;
}

void MeshLoadInterleaved(Mesh *mesh)
{
    std::cout << "Loading mesh to GPU" << std::endl;
    if (mesh->gpu_loaded)
    {
        std::cerr << "Mesh already loaded on GPU" << std::endl;
        return;
    }

    if (!mesh->is_interleaved)
    {
        std::cerr << "Mesh is not interleaved" << std::endl;
        return;
    }

    glGenVertexArrays(1, &mesh->VAO);
    glGenBuffers(1, &mesh->VBO);

    // Bind VAO
    glBindVertexArray(mesh->VAO);

    // Load Data to GPU
    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh->n_vertices * 5 * sizeof(float), mesh->vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Unbind VAO
    glBindVertexArray(0);
    // Unbind VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    mesh->gpu_loaded = true;
}

void MeshLoad(Mesh *mesh)
{
    std::cout << "Loading mesh to GPU" << std::endl;
    if (mesh->gpu_loaded)
    {
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Color buffer
    if (mesh->has_vertex_colors)
    {
        glGenBuffers(1, &mesh->CBO);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->CBO);
        glBufferData(GL_ARRAY_BUFFER, mesh->n_vertices * 3 * sizeof(float), mesh->colors, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);
    }

    // Texture Coords buffer
    if (mesh->has_texture_coords)
    {
        glGenBuffers(1, &mesh->TBO);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->TBO);
        glBufferData(GL_ARRAY_BUFFER, mesh->n_vertices * 2 * sizeof(float), mesh->texcoords, GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(2);
    }

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->n_indices * sizeof(unsigned int), mesh->indices, GL_STATIC_DRAW);

    mesh->gpu_loaded = true;
}

void MeshFree(Mesh *mesh)
{
    if (mesh->gpu_loaded)
    {
        glDeleteVertexArrays(1, &mesh->VAO);
        glDeleteBuffers(1, &mesh->VBO);
        glDeleteBuffers(1, &mesh->EBO);
        if (mesh->has_vertex_colors)
        {
            glDeleteBuffers(1, &mesh->CBO);
        }
    }

    delete[] mesh->vertices;
    delete[] mesh->colors;
    delete[] mesh->indices;

    mesh->gpu_loaded = false;
}

unsigned int LoadTexture(const char *path)
{
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
    if (data)
    {
        GLenum format;
        if (nrChannels == 1)
        {
            format = GL_RED;
        }
        else if (nrChannels == 3)
        {
            std::cout << "RGB" << std::endl;
            format = GL_RGB;
        }
        else if (nrChannels == 4)
        {
            std::cout << "RGBA" << std::endl;
            format = GL_RGBA;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Configure Mipmaps
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 3);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 3);
    }
    else
    {
        std::cerr << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    return texture;
}

void DrawMesh(Mesh mesh)
{
    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, mesh.n_indices, GL_UNSIGNED_INT, 0);
}

void DrawMeshEx(Mesh mesh, unsigned int mode)
{
    if (!mesh.gpu_loaded)
    {
        std::cerr << "Mesh not loaded on GPU" << std::endl;
        return;
    }
    glBindVertexArray(mesh.VAO);
    if (mesh.is_interleaved)
    {
        glDrawArrays(mode, 0, mesh.n_vertices);
    }
    else
    {
        glDrawElements(mode, mesh.n_indices, GL_UNSIGNED_INT, 0);
    }
}

void SetUniform4f(int shaderProgram, const char *name, float v0, float v1, float v2, float v3)
{
    int location = glGetUniformLocation(shaderProgram, name);
    glUniform4f(location, v0, v1, v2, v3);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
int main()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create a windowed mode window and its OpenGL context
    GLFWwindow *window = glfwCreateWindow(800, 600, "OpenGL Starter", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Set the viewport
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Configure OpenGL
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);

    // My Stuff
    Shader shader("src/shaders/default/vertex.glsl", "src/shaders/default/fragment.glsl");

    // Load Texture
    unsigned int texture1 = LoadTexture("assets/textures/container.jpg");
    unsigned int texture2 = LoadTexture("assets/textures/awesomeface.png");

    // Setup vertex data
    // Mesh mesh = MakeMeshGrid(10, 10, true);
    // Mesh mesh = MakeTriangle(true);
    // Mesh mesh = MakeTriangleTextured(true);
    // Mesh mesh = MakePlaneTextured(false);
    Mesh mesh = MakeCubeTextured(false);
    MeshLoadInterleaved(&mesh);

    // FPS Control
    double target_fps = 60.0;
    double target_frame_time = 1.0 / target_fps;

    double fps_counter_time = 0.0; // Accumulator to track time for FPS reporting
    int frame_count = 0;           // Number of frames since last report

    // Test glm
    glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));
    vec = trans * vec;
    std::cout << vec.x << vec.y << vec.z << std::endl;

    // Matrix Transformations
    // clip = projection * view * model * vec

    // Model Matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    // View Matrix
    glm::mat4 view = glm::mat4(1.0f);
    // note that we're translating the scene in the reverse direction of where we want to move
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    // Projection Matrix
    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    // Set Uniforms
    shader.use();

    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    shader.setMat4("model", model);

    // Test
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)};

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        double start_time = glfwGetTime();

        // input
        processInput(window);

        // rendering commands
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();
        shader.setInt("texture1", 0);
        shader.setInt("texture2", 1);

        model = glm::rotate(model, sin((float)glfwGetTime()) * glm::radians(1.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        shader.setMat4("model", model);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // DrawMeshEx(mesh, GL_TRIANGLES);

        glBindVertexArray(mesh.VAO);
        for(unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i; 
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Calculate frame time and sleep if needed
        double frame_time = glfwGetTime() - start_time;
        if (frame_time < target_frame_time)
        {
            std::this_thread::sleep_for(
                std::chrono::duration<double>(target_frame_time - frame_time));
        }

        // Increment frame counter
        frame_count++;
        fps_counter_time += glfwGetTime() - start_time;

        // Print FPS every second
        if (fps_counter_time >= 1.0)
        {
            std::cout << "FPS: " << frame_count << std::endl;
            frame_count = 0;
            fps_counter_time = 0.0;
        }
    }

    // Cleanup
    MeshFree(&mesh);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
