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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Globals
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

struct Mesh
{
    float *vertices;
    float *colors;
    float *texcoords;
    unsigned int *indices;
    unsigned int n_vertices;
    unsigned int n_indices;
    unsigned int VBO, VAO, EBO, CBO, TBO;
    unsigned int n_vertex_floats = 7;
    bool emit_light = false;
    float emit_strength = 1.0f;
    bool gpu_loaded = false;
};

void LoadMesh(Mesh *mesh);

Mesh GenerateMeshCube() {
    Mesh mesh;
    mesh.n_vertex_floats = 8;

    // Define interleaved vertex data: x, y, z, u, v, nx, ny, nz
    float cube_vertices[] = {
        // Front face
        -1, -1, -1, 0, 0,  0,  0, -1, // Bottom-left
        1, -1, -1, 1, 0,  0,  0, -1, // Bottom-right
        1,  1, -1, 1, 1,  0,  0, -1, // Top-right
        -1,  1, -1, 0, 1,  0,  0, -1, // Top-left
        // Back face
        -1, -1,  1, 0, 0,  0,  0,  1, // Bottom-left
        1, -1,  1, 1, 0,  0,  0,  1, // Bottom-right
        1,  1,  1, 1, 1,  0,  0,  1, // Top-right
        -1,  1,  1, 0, 1,  0,  0,  1, // Top-left
        // Left face
        -1, -1,  1, 0, 0, -1,  0,  0, // Bottom-left
        -1, -1, -1, 1, 0, -1,  0,  0, // Bottom-right
        -1,  1, -1, 1, 1, -1,  0,  0, // Top-right
        -1,  1,  1, 0, 1, -1,  0,  0, // Top-left
        // Right face
        1, -1, -1, 0, 0,  1,  0,  0, // Bottom-left
        1, -1,  1, 1, 0,  1,  0,  0, // Bottom-right
        1,  1,  1, 1, 1,  1,  0,  0, // Top-right
        1,  1, -1, 0, 1,  1,  0,  0, // Top-left
        // Top face
        -1,  1, -1, 0, 0,  0,  1,  0, // Bottom-left
        1,  1, -1, 1, 0,  0,  1,  0, // Bottom-right
        1,  1,  1, 1, 1,  0,  1,  0, // Top-right
        -1,  1,  1, 0, 1,  0,  1,  0, // Top-left
        // Bottom face
        -1, -1, -1, 0, 0,  0, -1,  0, // Bottom-left
        1, -1, -1, 1, 0,  0, -1,  0, // Bottom-right
        1, -1,  1, 1, 1,  0, -1,  0, // Top-right
        -1, -1,  1, 0, 1,  0, -1,  0, // Top-left
    };


    // Define indices for drawing triangles
    unsigned int cube_indices[] = {
        // Front face
        0, 1, 2, 2, 3, 0,
        // Back face
        4, 5, 6, 6, 7, 4,
        // Left face
        8, 9, 10, 10, 11, 8,
        // Right face
        12, 13, 14, 14, 15, 12,
        // Top face
        16, 17, 18, 18, 19, 16,
        // Bottom face
        20, 21, 22, 22, 23, 20,
    };


    // Allocate memory for the mesh data
    mesh.n_vertices = 24;
    mesh.n_indices = 36;

    // for (int i = 0; i < mesh.n_vertices; ++i) {
    //     float nx = mesh.vertices[i * mesh.n_vertex_floats + 5];
    //     float ny = mesh.vertices[i * mesh.n_vertex_floats + 6];
    //     float nz = mesh.vertices[i * mesh.n_vertex_floats + 7];
    //     std::cout << "Normal[" << i << "] = (" << nx << ", " << ny << ", " << nz << ")" << std::endl;
    // }


    mesh.vertices = new float[mesh.n_vertices * mesh.n_vertex_floats]; // 5 values per vertex (x, y, z, u, v)
    memcpy(mesh.vertices, cube_vertices, sizeof(cube_vertices));

    mesh.indices = new unsigned int[mesh.n_indices];
    memcpy(mesh.indices, cube_indices, sizeof(cube_indices));

    return mesh;
}



void LoadMesh(Mesh *mesh) {
    std::cout << "Loading mesh to GPU" << std::endl;
    if (mesh->gpu_loaded) {
        std::cerr << "Mesh already loaded on GPU" << std::endl;
        return;
    }

    glGenVertexArrays(1, &mesh->VAO);
    glGenBuffers(1, &mesh->VBO);
    glGenBuffers(1, &mesh->EBO);

    glBindVertexArray(mesh->VAO);

    // Position and Texture Coordinates (Interleaved Buffer)
    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh->n_vertices * mesh->n_vertex_floats * sizeof(float), mesh->vertices, GL_STATIC_DRAW);

    // Vertex Position (x, y, z)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, mesh->n_vertex_floats * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Texture Coordinates (u, v)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, mesh->n_vertex_floats * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Normal Vectors (nx, ny, nz)
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, mesh->n_vertex_floats * sizeof(float), (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // Index Buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->n_indices * sizeof(unsigned int), mesh->indices, GL_STATIC_DRAW);

    glBindVertexArray(0);

    mesh->gpu_loaded = true;
    std::cout << "Mesh successfully loaded to GPU" << std::endl;
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
        LoadMesh(&mesh);
    }
    return mesh;
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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 3);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 3);
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
    glDrawElements(mode, mesh.n_indices, GL_UNSIGNED_INT, 0);
    
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

  
    // Configure OpenGL
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);

    // My Stuff
    Shader lightingShader("lighting");
    Shader lightCubeShader("lightCube");

    lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
    lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);



    // Load Texture
    unsigned int texture1 = LoadTexture("assets/textures/container.jpg");
    unsigned int texture2 = LoadTexture("assets/textures/awesomeface.png");

    // Setup vertex data
    // Mesh mesh = MakeMeshGrid(10, 10, true);
    // Mesh mesh = MakeTriangle(true);
    // Mesh mesh = MakeTriangleTextured(true);
    // Mesh mesh = MakePlaneTextured(false);
    Mesh mesh = GenerateMeshCube();
    Mesh meshLight = GenerateMeshCube();
    LoadMesh(&mesh);
    LoadMesh(&meshLight);

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
    // model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    glm::mat4 modelLight = glm::mat4(1.0f);
    modelLight = glm::translate(modelLight, glm::vec3(1.2f, 1.0f, 2.0f));
    modelLight = glm::scale(modelLight, glm::vec3(0.2f)); // Make it a smaller cube

    // View Matrix
    glm::mat4 view;
    view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),
                       glm::vec3(0.0f, 0.0f, 0.0f),
                       glm::vec3(0.0f, 1.0f, 0.0f));

    // Projection Matrix
    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    // Set Uniforms
    // shader.use();
    // shader.setMat4("view", view);
    // shader.setMat4("projection", projection);
    // shader.setMat4("model", model);

    lightingShader.use();
    lightingShader.setMat4("model", model);
    lightingShader.setMat4("view", view);
    lightingShader.setMat4("projection", projection);
    lightingShader.setVec3("lightPos", 1.2f, 1.0f, 2.0f);
    lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
    lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

    lightCubeShader.setMat4("model", modelLight);
    lightCubeShader.setMat4("view", view);
    lightCubeShader.setMat4("projection", projection);


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
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;  

        // input
        processInput(window);

        // rendering commands
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("view", view);
        lightCubeShader.setMat4("view", view);

        // Draw Light
        lightingShader.use();
        DrawMesh(mesh);


        // Draw Light
        lightCubeShader.use();
        DrawMesh(meshLight);


    
        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Calculate frame time and sleep if needed
        double frame_time = glfwGetTime() - currentFrame;
        if (frame_time < target_frame_time)
        {
            std::this_thread::sleep_for(
                std::chrono::duration<double>(target_frame_time - frame_time));
        }

        // Increment frame counter
        frame_count++;
        fps_counter_time += glfwGetTime() - currentFrame;

        // Print FPS every second
        if (fps_counter_time >= 1.0)
        {
            std::cout << "FPS: " << frame_count << std::endl;
            frame_count = 0;
            fps_counter_time = 0.0;
        }
    }

    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
