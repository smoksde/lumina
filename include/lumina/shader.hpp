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
        std::unordered_map<std::string, GLint> uniform_locations;

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

            uniform_locations = getUniformLocations(shader_id);

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

        void setInt(const std::string& name, int value)
        {
            auto it = uniform_locations.find(name);
            if (it != uniform_locations.end())
                glUniform1i(it->second, value);
        }

        void setFloat(const std::string& name, float value)
        {
            auto it = uniform_locations.find(name);
            if (it != uniform_locations.end())
                glUniform1f(it->second, value);
        }

        void setVec2(const std::string& name, const glm::vec2& value)
        {
            auto it = uniform_locations.find(name);
            if (it != uniform_locations.end())
                glUniform2fv(it->second, 1, glm::value_ptr(value));
        }

        void setVec3(const std::string& name, const glm::vec3& value)
        {
            auto it = uniform_locations.find(name);
            if (it != uniform_locations.end())
                glUniform3fv(it->second, 1, glm::value_ptr(value));
        }

        void setVec4(const std::string& name, const glm::vec4& value)
        {
            auto it = uniform_locations.find(name);
            if (it != uniform_locations.end())
                glUniform4fv(it->second, 1, glm::value_ptr(value));
        }

        void setMat4(const std::string& name, const glm::mat4& value)
        {
            auto it = uniform_locations.find(name);
            if (it != uniform_locations.end())
                glUniformMatrix4fv(it->second, 1, GL_FALSE, glm::value_ptr(value));
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

        std::unordered_map<std::string, GLint> getUniformLocations(GLuint program)
        {
            std::unordered_map<std::string, GLint> uniform_locations;
            GLint uniform_count;
            glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniform_count);
            
            for (GLint i = 0; i < uniform_count; ++i)
            {
                char name[256];
                GLsizei length;
                GLint size;
                GLenum type;
                glGetActiveUniform(program, i, sizeof(name), &length, &size, &type, name);
                GLint location = glGetUniformLocation(program, name);
                if (location != -1)
                {
                    uniform_locations[name] = location;
                }
            }
            return uniform_locations;
        }

        void setUniform(const std::string& name, float value)
        {
            if (uniform_locations.find(name) != uniform_locations.end())
            {
                glUniform1f(uniform_locations[name], value);
            }
            else
            {
                std::cerr << "Warning: Uniform '" << name << "' not found!" << std::endl;
            }
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