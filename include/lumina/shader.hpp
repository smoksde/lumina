#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace lumina {

class Shader {
public:
    GLuint shader_id;

    Shader(const std::unordered_map<std::string, std::string>& shader_paths) {
        shader_id = glCreateProgram();

        std::unordered_map<std::string, GLenum> shader_types = {
            {"vertex", GL_VERTEX_SHADER},
            {"fragment", GL_FRAGMENT_SHADER},
            {"geometry", GL_GEOMETRY_SHADER},
            {"compute", GL_COMPUTE_SHADER}
        };

        std::vector<GLuint> compiled_shaders;

        for (const auto& [type, path] : shader_paths) {
            if (shader_types.count(type)) {
                GLuint shader = compile(parse(path.c_str()), shader_types[type]);
                if (shader != 0) {
                    glAttachShader(shader_id, shader);
                    compiled_shaders.push_back(shader);
                }
            }
        }

        glLinkProgram(shader_id);
        checkLinkErrors(shader_id);

        // Cache uniforms once after linking
        cacheUniformLocations();

        for (GLuint shader : compiled_shaders) {
            glDeleteShader(shader);
        }
    }

    virtual ~Shader() {
        glDeleteProgram(shader_id);
    }

    void bind() const {
        glUseProgram(shader_id);
    }

    void unbind() const {
        glUseProgram(0);
    }

    GLint getUniformLocation(const std::string& name) {
        auto it = uniform_locations.find(name);
        if (it != uniform_locations.end())
            return it->second;

        GLint loc = glGetUniformLocation(shader_id, name.c_str());
        uniform_locations[name] = loc;
        return loc;
    }

    template<typename T>
    void setUniform(const std::string& name, const T& value) {
        static_assert(sizeof(T) == 0, "Unsupported uniform type");
    }

private:
    std::unordered_map<std::string, GLint> uniform_locations;

    void cacheUniformLocations() {
        GLint count;
        glGetProgramiv(shader_id, GL_ACTIVE_UNIFORMS, &count);

        for (GLint i = 0; i < count; ++i) {
            char name[256];
            GLsizei length;
            GLint size;
            GLenum type;
            glGetActiveUniform(shader_id, i, sizeof(name), &length, &size, &type, name);

            GLint location = glGetUniformLocation(shader_id, name);
            if (location != -1)
                uniform_locations[name] = location;
        }
    }

    GLuint compile(const std::string& source, GLenum type) {
        GLuint id = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(id, 1, &src, nullptr);
        glCompileShader(id);

        GLint status;
        glGetShaderiv(id, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE) {
            GLint length;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
            std::string log(length, ' ');
            glGetShaderInfoLog(id, length, &length, &log[0]);
            std::cerr << "Shader compile error: " << log << std::endl;
            return 0;
        }
        return id;
    }

    void checkLinkErrors(GLuint program) {
        GLint success;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            char info_log[512];
            glGetProgramInfoLog(program, 512, nullptr, info_log);
            std::cerr << "Shader link error: " << info_log << std::endl;
        }
    }

    std::string parse(const char* filename) {
        FILE* file = fopen(filename, "rb");
        if (!file) {
            std::cerr << "File not found: " << filename << std::endl;
            return "";
        }
        fseek(file, 0, SEEK_END);
        size_t size = ftell(file);
        rewind(file);

        std::string contents(size, '\0');
        fread(contents.data(), 1, size, file);
        fclose(file);

        return contents;
    }
};

// ----------------------------
// Template specializations
// ----------------------------

template<>
inline void Shader::setUniform<int>(const std::string& name, const int& value) {
    glUniform1i(getUniformLocation(name), value);
}

template<>
inline void Shader::setUniform<float>(const std::string& name, const float& value) {
    glUniform1f(getUniformLocation(name), value);
}

template<>
inline void Shader::setUniform<glm::vec2>(const std::string& name, const glm::vec2& value) {
    glUniform2fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

template<>
inline void Shader::setUniform<glm::vec3>(const std::string& name, const glm::vec3& value) {
    glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

template<>
inline void Shader::setUniform<glm::vec4>(const std::string& name, const glm::vec4& value) {
    glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

template<>
inline void Shader::setUniform<glm::mat4>(const std::string& name, const glm::mat4& value) {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

}
