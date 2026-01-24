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
                shader->setUniform("model", model);

                for (const auto& [name, val] : uniforms)
                {
                    std::visit([&](auto&& v) {
                        shader->setUniform(name, v);
                    }, val);
                }
            }
            
        private:
            std::shared_ptr<Shader> shader;
            std::unordered_map<std::string, UniformValue> uniforms;
    };
}


