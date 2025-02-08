#pragma once

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
            std::vector<float> vertices;
            std::vector<unsigned int> indices;
            parseFile(filename, vertices, indices);
            auto vertices_ptr = std::make_shared<std::vector<float>>(vertices);
            auto indices_ptr = std::make_shared<std::vector<unsigned int>>(indices);
            return std::make_shared<Mesh>(vertices_ptr, vertices.size(), indices_ptr, indices.size());
        }

        private:
        static void parseFile(const std::string& filename, std::vector<float>& vertices, std::vector<unsigned int>& indices)
        {
            unsigned int num_vertices = 0;
            unsigned int num_indices = 0;

            std::ifstream file(filename);
            std::string line;

            bool first = true;

            if (file.is_open())
            {
                while (std::getline(file, line))
                {   
                    if (line.empty())
                    {
                        first = false;
                    }
                    else
                    {
                        std::stringstream ss(line);
                        std::string x, y, z;
                        if (ss >> x >> y >> z)
                        {
                            if (first)
                            {
                                vertices.push_back(std::stof(x));
                                vertices.push_back(std::stof(y));
                                vertices.push_back(std::stof(z));
                                num_vertices++;
                            }
                            else
                            {
                                indices.push_back(static_cast<unsigned int>(std::stoi(x)));
                                indices.push_back(static_cast<unsigned int>(std::stoi(y)));
                                indices.push_back(static_cast<unsigned int>(std::stoi(z)));
                                num_indices += 3;
                            }
                        }
                    }
                }
                file.close();
            }
            else
            {
                std::cerr << "Unable to open file: " << filename << std::endl;
            }

            return;
        }
    };
}