#version 330 core

layout(location = 0) in vec3 aPosition;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec4 vPositionClipSpace;

void main() {
    vec4 worldPosition = uModel * vec4(aPosition, 1.0);
    vPositionClipSpace = uProjection * uView * worldPosition; // Pass clip-space position
    gl_Position = vPositionClipSpace;
}