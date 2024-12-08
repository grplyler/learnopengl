cmake_minimum_required(VERSION 3.15)

# Project name and version
project(opengl-starter VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the main executable
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/shader.h
    src/camera.h
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    vendor/stb
    vendor/glm
    vendor/glfw/include
    vendor/glad/include
    vendor/imgui
    vendor/imgui/backends
    vendor/assimp/include # Assimp include path
)

# Add GLFW as a subdirectory
add_subdirectory(vendor/glfw)

# # Link GLFW to the main executable
# target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

# Optionally, enable support for the Vulkan backend in GLFW
option(GLFW_USE_VULKAN "Enable Vulkan support in GLFW" OFF)

# Add GLAD as a static library
add_library(glad STATIC vendor/glad/src/glad.c)
target_include_directories(glad PUBLIC vendor/glad/include)

# # Link GLAD to the main executable
# target_link_libraries(${PROJECT_NAME} PRIVATE glad)

# Add ImGui as a static library
add_library(imgui STATIC
    vendor/imgui/imgui.cpp
    vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_widgets.cpp
    vendor/imgui/imgui_tables.cpp
    vendor/imgui/imgui_demo.cpp
    vendor/imgui/backends/imgui_impl_glfw.cpp
    vendor/imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
    vendor/imgui
    vendor/imgui/backends
    vendor/glfw/include # GLFW include path
)

# Link GLFW and GLAD to ImGui
target_link_libraries(imgui PRIVATE glfw glad)

# Define the OpenGL loader for ImGui
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)

# Link ImGui to the main executable
target_link_libraries(${PROJECT_NAME} PRIVATE imgui)

# Find Assimp
find_package(assimp REQUIRED)

# Include Assimp's headers
target_include_directories(${PROJECT_NAME} PRIVATE ${ASSIMP_INCLUDE_DIRS})

# Link Assimp to your executable
target_link_libraries(${PROJECT_NAME} PRIVATE ${ASSIMP_LIBRARIES})

# Platform-specific OpenGL linking
if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
elseif (APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
elseif (UNIX)
    find_package(OpenGL REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL X11 pthread)
endif()
