#pragma once

#include <fstream>

namespace lumina
{
    class GeometryLoader
    {
        public:
        GeometryLoader()
        {

        }
        
        static std::shared_ptr<Mesh> loadGeometryFromFile(const std::string& filename)
        {
            std::vector<float> positions;
            std::vector<float> colors;
            std::vector<float> uvs;
            std::vector<unsigned int> indices;

            parseFile(filename, positions, colors, uvs, indices);

            auto pos_ptr = std::make_shared<std::vector<float>>(positions);
            auto ind_ptr = std::make_shared<std::vector<unsigned int>>(indices);
            std::shared_ptr<std::vector<float>> colors_ptr = colors.empty() ? nullptr : std::make_shared<std::vector<float>>(colors);
            std::shared_ptr<std::vector<float>> uvs_ptr = uvs.empty() ? nullptr : std::make_shared<std::vector<float>>(uvs);

            return std::make_shared<Mesh>(pos_ptr, ind_ptr, colors_ptr, uvs_ptr);
        }

        private:
        enum class Section
        {
            None,
            Positions,
            UVs,
            Indices
        };

        static void parseFile(const std::string& filename,
                              std::vector<float>& positions,
                              std::vector<float>& colors,
                              std::vector<float>& uvs,
                              std::vector<unsigned int>& indices)
        {
            std::ifstream file(filename);
            std::string line;
            Section currentSection = Section::None;

            if (!file.is_open())
            {
                std::cerr << "Unable to open file: " << filename << std::endl;
                return;
            }

            while (std::getline(file, line))
            {
                // Trim and ignore empty lines
                if (line.empty() || line[0] == '\r')
                    continue;

                if (line[0] == '#')
                {
                    if (line == "#positions") currentSection = Section::Positions;
                    else if (line == "#uvs") currentSection = Section::UVs;
                    else if (line == "#indices") currentSection = Section::Indices;
                    else currentSection = Section::None;
                    continue;
                }

                std::stringstream ss(line);
                switch (currentSection)
                {
                    case Section::Positions: {
                        float x, y, z, r, g, b;
                        if (ss >> x >> y >> z)
                        {
                            positions.push_back(x);
                            positions.push_back(y);
                            positions.push_back(z);
                            
                            // Try to read color data (optional)
                            if (ss >> r >> g >> b)
                            {
                                colors.push_back(r);
                                colors.push_back(g);
                                colors.push_back(b);
                            }
                            else
                            {
                                // Default to white if no color data
                                colors.push_back(1.0f);
                                colors.push_back(1.0f);
                                colors.push_back(1.0f);
                            }
                        }
                        break;
                    }
                    case Section::UVs: {
                        float u, v;
                        if (ss >> u >> v)
                        {
                            uvs.push_back(u);
                            uvs.push_back(v);
                        }
                        break;
                    }
                    case Section::Indices: {
                        unsigned int i1, i2, i3;
                        if (ss >> i1 >> i2 >> i3)
                        {
                            indices.push_back(i1);
                            indices.push_back(i2);
                            indices.push_back(i3);
                        }
                        break;
                    }
                    default:
                        std::cerr << "Unknown section or malformed line: " << line << "\n";
                        break;
                }
            }

            file.close();
        }
    };
}