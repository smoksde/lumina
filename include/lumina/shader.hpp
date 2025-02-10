#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "GL/glew.h"
namespace lumina
{
    class Shader
    {
        public:
        GLuint shader_id;

        Shader(const std::unordered_map<std::string, std::string>& shader_paths)
        {
            shader_id = glCreateProgram();

            std::unordered_map<std::string, GLenum> shader_types = 
            {
                {"vertex", GL_VERTEX_SHADER},
                {"fragment", GL_FRAGMENT_SHADER},
                {"geometry", GL_GEOMETRY_SHADER},
                {"compute", GL_COMPUTE_SHADER}
            };

            std::vector<GLuint> compiled_shaders;

            for (const auto& [type, path] : shader_paths)
            {
                if (shader_types.find(type) != shader_types.end())
                {
                    GLuint shader = compile(parse(path.c_str()), shader_types[type]);
                    if (shader != 0)
                    {
                        glAttachShader(shader_id, shader);
                        compiled_shaders.push_back(shader);
                    }
                }
                else
                {
                    std::cerr << "Warning: Unknown shader type '" << type << "' for shader program." << std::endl;
                }
            }

            glLinkProgram(shader_id);
            checkLinkErrors(shader_id);

            for (GLuint shader : compiled_shaders)
            {
                glDeleteShader(shader);
            }
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

        void checkLinkErrors(GLuint program)
        {
            GLint success;
            glGetProgramiv(program, GL_LINK_STATUS, &success);
            if (!success)
            {
                char info_log[512];
                glGetProgramInfoLog(program, 512, nullptr, info_log);
                std::cerr << "Shader program linking failed: " << info_log << std::endl;
            }
        }

        std::string parse(const char* filename)
        {
            FILE* file;
            file = fopen(filename, "rb");
            if(file == nullptr){
                std::cout << "File " << filename << " not found" << std::endl;
                return "";
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
        
        GLuint createShader(const char* vertex_shader_filename, const char* fragment_shader_filename = nullptr)
        {
            std::string vertex_shader_source = parse(vertex_shader_filename);
            
            GLuint program = glCreateProgram();
            

            GLuint vert = compile(vertex_shader_source, GL_VERTEX_SHADER);
            glAttachShader(program, vert);

            GLuint frag = 0;
            if (fragment_shader_filename && strlen(fragment_shader_filename) > 0)
            {
                std::string fragment_shader_source = parse(fragment_shader_filename);
                frag = compile(fragment_shader_source, GL_FRAGMENT_SHADER);
                glAttachShader(program, frag);
            }

            glLinkProgram(program);

            glDetachShader(program, vert);
            glDeleteShader(vert);

            if (frag)
            {
                glDetachShader(program, frag);
                glDeleteShader(frag);
            }
            return program;
        }
    };
}