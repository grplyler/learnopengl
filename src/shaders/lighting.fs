
#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
  
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    // Diffuse lighting
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Ambient lighting
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Final color
    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0);
    // FragColor = vec4((nn + 1.0) * 0.5, 1.0); // Normalize and map [-1, 1] to [0, 1]
}