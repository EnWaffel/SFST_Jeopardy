#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec4 col;

void main()
{
    vec2 c = vec2(TexCoords.x, TexCoords.y);
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, c).r);
    color = vec4(1.0, 1.0, 1.0, 1.0) * sampled;
    color *= col;
}