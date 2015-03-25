#version 330

uniform float intensityFactor;

in vec3 pColor;
out vec3 finalColor;

void main() {
	finalColor = clamp(intensityFactor*pColor, 0, 1);
}