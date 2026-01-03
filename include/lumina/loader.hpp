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

        static nlohmann::json loadJson(const std::string& file_path)
        {
            std::ifstream file(file_path);
            if (!file)
            {
                std::cerr << "Error: Could not open " << file_path << std::endl;
                return {};
            }
            nlohmann::json root;
            try
            {
                file >> root;
            }
            catch (const std::exception& e)
            {
                std::cerr << "Error parsing JSON: " << e.what() << std::endl;
                return {};
            }
            return root;
        }

        static std::unordered_map<std::string, std::shared_ptr<lumina::Shader>> loadShaders(const std::string& file_path)
        {
            nlohmann::json root = loadJson(file_path);

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

        static std::unordered_map<std::string, std::shared_ptr<lumina::Mesh>> loadMeshes(const std::string& file_path)
        {
            nlohmann::json root = loadJson(file_path);
            if (!root.contains("meshes"))
            {
                std::cerr << "Error: 'meshes' key not found in JSON file." << std::endl;
                return {};
            }

            std::unordered_map<std::string, std::shared_ptr<lumina::Mesh>> meshes;

            for (const auto& [name, mesh_path] : root["meshes"].items())
            {
                if (!mesh_path.is_string())
                {
                    std::cerr << "Error: Mesh path for '" << mesh_path << "' in '" << name << "' is not a valid string." << std::endl;
                    continue;
                }
                meshes[name] = GeometryLoader::loadGeometryFromFile(mesh_path);
            }

            return meshes;
        }

        static std::unordered_map<std::string, glm::vec4> loadColors(const std::string& file_path)
        {
            nlohmann::json root = loadJson(file_path);

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

        static std::unordered_map<std::string, std::shared_ptr<lumina::Texture>>
        loadTextures(const std::string& directory)
        {
            namespace fs = std::filesystem;
            std::unordered_map<std::string, std::shared_ptr<lumina::Texture>> textures;
            if (!fs::exists(directory) || !fs::is_directory(directory))
            {
                std::cerr << "Error: Texture directory does not exist: "
                << directory << std::endl;
                return textures;
            }

            stbi_set_flip_vertically_on_load(true);

            for (const auto& entry : fs::directory_iterator(directory))
            {
                if (!entry.is_regular_file())
                    continue;

                const fs::path& path = entry.path();
                std::string ext = path.extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

                // Supported formats
                if (ext != ".png" && ext != ".jpg" && ext != ".jpeg" && ext != ".bmp")
                    continue;

                std::string name = path.stem().string(); // filename without extension

                auto texture = std::make_shared<lumina::Texture>();
                if (!texture->loadFromFile(path.string()))
                {
                    std::cerr << "Failed to load texture: " << path << std::endl;
                    continue;
                }

                textures[name] = texture;

                std::cout << "Loaded texture: " << name << " (" << path << ")\n";
            }

            return textures;
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