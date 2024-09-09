#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 _texCoord;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 cam;

void main() {
    gl_Position = view * cam * model * vec4(pos, 1.0);
	TexCoords = _texCoord;
}