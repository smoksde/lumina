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

        static std::unordered_map<std::string, glm::vec4> loadColors(const std::string& file_path)
        {
            std::ifstream file(file_path);
            if (!file)
            {
                std::cerr << "Error: Could not open " << file_path << std::endl;
                return {};
            }

            nlohmann::json root;
            file >> root;

            std::cout << "Loaded JSON: " << root.dump(4) << std::endl;
            if (!root.contains("colors"))
            {
                std::cerr << "Error: 'colors' key not found in JSON file." << std::endl;
                return {};
            }

            std::unordered_map<std::string, glm::vec4> colors;
            for (const auto& [name, color] : root["colors"].items())
            {
                if (color.is_array() && color.size() == 4)
                {
                    // RGBA format
                    colors[name] = glm::vec4(color[0], color[1], color[2], color[3]);
                }
                else if (color.is_string())
                {
                    colors[name] = hexToRGBA(color.get<std::string>());
                }
                else
                {
                    std::cerr << "Warning: Invalid color format for " << name << std::endl;
                }
            }
            return colors;
        }

        private:

        static glm::vec4 hexToRGBA(const std::string& hex)
        {
            if (hex.size() != 7 || hex[0] != '#')
            {
                std::cerr << "Error: Invalid hex color " << hex << std::endl;
                return glm::vec4(1.0f);
            }
            int r, g, b;
            sscanf(hex.c_str(), "#%02x%02x%02x", &r, &g, &b);
            return glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
        }
    };
}