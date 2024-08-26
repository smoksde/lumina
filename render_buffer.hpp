#pragma once

#include <GL/glew.h>

class RenderBuffer {
public:
    GLuint VAO;
    GLuint VBO;
    GLuint instanceVBO;

    RenderBuffer() {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
    }

    void setVertexData(const void* data, size_t size, GLenum usage = GL_STATIC_DRAW) {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, size, data, usage);
    }

    void setVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer) {
        glVertexAttribPointer(index, size, type, normalized, stride, pointer);
        glEnableVertexAttribArray(index);
    }

    void setInstanceData(size_t numInstances, size_t instanceDataSize, GLenum usage = GL_DYNAMIC_DRAW) {
        glGenBuffers(1, &instanceVBO);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, instanceDataSize * numInstances, nullptr, usage);
    }

    void setInstanceSubData(GLintptr offset, GLsizeiptr size, const void* data)
    {
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
    }
    
    void setInstanceAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer, GLuint divisor) {
        glVertexAttribPointer(index, size, type, normalized, stride, pointer);
        glEnableVertexAttribArray(index);
        glVertexAttribDivisor(index, divisor);
    }

    void bind() {
        glBindVertexArray(VAO);
    }

    void unbind() {
        glBindVertexArray(0);
    }

    ~RenderBuffer() {
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &instanceVBO);
        glDeleteVertexArrays(1, &VAO);
    }
};
