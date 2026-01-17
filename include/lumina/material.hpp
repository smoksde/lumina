#pragma once

#include <memory>
#include <variant>
#include <unordered_map>
#include <string>
#include <glm/glm.hpp>
#include "shader.hpp"

namespace lumina
{
    
    using UniformValue = std::variant<int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat4>; 

    class Material
    {
        public:
            Material(std::shared_ptr<Shader> shader)
            : shader(shader)
            {

            }
            void setUniform(const std::string& name, const UniformValue& value)
            {
                uniforms[name] = value;
            }
            void bind(const glm::mat4& model) const
            {
                if (!shader) return;
                shader->bind();
                shader->setMat4("model", model);

                for (const auto& [name, val] : uniforms)
                {
                    std::visit([&](auto&& v) {
                        using T = std::decay_t<decltype(v)>;
                        if constexpr (std::is_same_v<T, int>)
                            shader->setInt(name, v);
                        else if constexpr (std::is_same_v<T, float>)
                            shader->setFloat(name, v);
                        else if constexpr (std::is_same_v<T, glm::vec2>)
                            shader->setVec2(name, v);
                        else if constexpr (std::is_same_v<T, glm::vec3>)
                            shader->setVec3(name, v);
                        else if constexpr (std::is_same_v<T, glm::vec4>)
                            shader->setVec4(name, v);
                        else if constexpr (std::is_same_v<T, glm::mat4>)
                            shader->setMat4(name, v);
                    }, val);
                }
            }
            
        private:
            std::shared_ptr<Shader> shader;
            std::unordered_map<std::string, UniformValue> uniforms;
    };
}


