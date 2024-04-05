#version 330 core
out vec4 FragColor;

uniform sampler2D tex;

in vec2 texCoord;

void main() {
	FragColor = texture(tex, texCoord);
}