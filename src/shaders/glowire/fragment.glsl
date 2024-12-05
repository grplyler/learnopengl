#version 330 core

in vec4 vPositionClipSpace;

out vec4 FragColor;

uniform vec2 uScreenSize;
uniform float uGlowIntensity;

void main() {
    // Convert clip-space to normalized device coordinates (NDC)
    vec2 ndc = vPositionClipSpace.xy / vPositionClipSpace.w;

    // Calculate distance from the center of the screen
    vec2 screenPos = (ndc * 0.5 + 0.5) * uScreenSize; // Map NDC to screen space
    float distanceToCenter = length(screenPos - uScreenSize * 0.5);

    // Glow intensity based on proximity to screen center
    float glowFactor = 1.0 - smoothstep(0.0, uScreenSize.x * 0.5, distanceToCenter);
    glowFactor = pow(glowFactor, 2.0) * uGlowIntensity;

    // Combine base cyan color with glow effect
    vec3 baseColor = vec3(0.0, 1.0, 1.0); // Cyan base
    vec3 glowColor = baseColor + glowFactor * vec3(0.2, 0.6, 1.0); // Add bluish glow

    FragColor = vec4(glowColor, 1.0);
}
