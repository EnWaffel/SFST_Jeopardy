#include "GLBuffers.h"
#include <glad/glad.h>

GLBuffers::GLBuffers()
{
    {
        glGenVertexArrays(1, &VAO_rect);
        glGenBuffers(1, &VBO_rect);

        float vertices[] = {
            // first triangle
             1.0f,  1.0f, 0.0f, 1.0, 1.0,  // top right
             1.0f,  0.0f, 0.0f, 1.0, 0.0,  // bottom right
             0.0f,  1.0f, 0.0f, 0.0, 1.0, // top left 
             // second triangle
              1.0f,  0.0f, 0.0f, 1.0, 0.0, // bottom right
              0.0f,  0.0f, 0.0f, 0.0, 0.0, // bottom left
              0.0f,  1.0f, 0.0f,  0.0, 1.0 // top left
        };

        glBindVertexArray(VAO_rect);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_rect);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
}

GLBuffers::~GLBuffers()
{
    glDeleteVertexArrays(1, &VAO_rect);
    glDeleteBuffers(1, &VBO_rect);
}
