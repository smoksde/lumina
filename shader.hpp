#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include "GL/glew.h"

struct Shader{
    Shader(const char* vertex_shader_filename, const char* fragment_shader_filename)
    {
        shader_id = createShader(vertex_shader_filename, fragment_shader_filename);
    }
    virtual ~Shader()
    {
        glDeleteProgram(shader_id);
    }

    void bind()
    {
        glUseProgram(shader_id);
    }
    void unbind()
    {
        glUseProgram(0);
    }

    GLuint shader_id;

private:

    GLuint compile(std::string shader_source, GLenum type)
    {
        GLuint id = glCreateShader(type);
        const char* src = shader_source.c_str();
        glShaderSource(id, 1, &src, 0);
        glCompileShader(id);

        int result;
        glGetShaderiv(id, GL_COMPILE_STATUS, &result);
        if(result != GL_TRUE){
            int length = 0;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
            char* message = new char[length];
            glGetShaderInfoLog(id, length, &length, message);
            std::cout << "Shader compilation error: " << message << std::endl;
            delete[] message;
            return 0;
        }
        return id;
    }

    std::string parse(const char* filename)
    {
        FILE* file;
        file = fopen(filename, "rb");
        if(file == nullptr){
            std::cout << "File " << filename << " not found" << std::endl;
            return 0;
        }

        std::string contents;
        fseek(file, 0, SEEK_END);
        size_t filesize = ftell(file);
        rewind(file);
        contents.resize(filesize);

        fread(&contents[0], 1, filesize, file);
        fclose(file);

        return contents;
    }
    
    GLuint createShader(const char* vertex_shader_filename, const char* fragment_shader_filename)
    {
        std::string vertex_shader_source = parse(vertex_shader_filename);
        std::string fragment_shader_source = parse(fragment_shader_filename);

        GLuint program = glCreateProgram();
        GLuint vert = compile(vertex_shader_source, GL_VERTEX_SHADER);
        GLuint frag = compile(fragment_shader_source, GL_FRAGMENT_SHADER);

        glAttachShader(program, vert);
        glAttachShader(program, frag);

        glLinkProgram(program);

        glDetachShader(program, vert);
        glDetachShader(program, frag);

        glDeleteShader(vert);
        glDeleteShader(frag);

        return program;
    }
};