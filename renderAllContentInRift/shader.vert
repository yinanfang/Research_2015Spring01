#version 330

uniform mat4 modelview;
uniform mat4 projection;
uniform mat4 camera;

in vec3 position;
in vec3 color;

out vec3 pColor;

void main() {
    vec4 wp = modelview * vec4(position, 1.0);
	gl_Position = projection * camera * wp;
	pColor = color;
}
