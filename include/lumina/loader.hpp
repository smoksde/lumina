#pragma once

#include <string>
#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>

namespace lumina
{
    class Loader
    {
        public:

        static std::unordered_map<std::string, std::shared_ptr<lumina::Shader>> loadShaders(const std::string& file_path)
        {
            std::ifstream file(file_path);
            if (!file)
            {
                std::cerr << "Error: Could not open " << file_path << std::endl;
                return {};
            }

            nlohmann::json root;
            file >> root;

            if (!root.contains("shaders"))
            {
                std::cerr << "Error: 'shaders' key not found in JSON file." << std::endl;
                return {};
            }

            std::unordered_map<std::string, std::shared_ptr<lumina::Shader>> shaders;

            for (const auto& [name, shader_data] : root["shaders"].items())
            {
                if (!shader_data.is_object())
                {
                    std::cerr << "Error: Invalid shader format for " << name << std::endl;
                    continue;
                }

                std::unordered_map<std::string, std::string> shader_paths;

                for (const auto& [shader_type, shader_path] : shader_data.items())
                {
                    if (!shader_path.is_string())
                    {
                        std::cerr << "Error: Shader path for '" << shader_type << "' in '" << name << "' is not a valid string." << std::endl;
                        continue;
                    }
                    shader_paths[shader_type] = shader_path;
                }
                shaders[name] = std::make_shared<lumina::Shader>(shader_paths);
            }
            return shaders;
        }
    };
}