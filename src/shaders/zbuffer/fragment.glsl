#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D depthTexture;

void main()
{
    float depth = texture(depthTexture, TexCoord).r; // Get depth value
    FragColor = vec4(vec3(depth), 1.0); // Convert depth to grayscale
}