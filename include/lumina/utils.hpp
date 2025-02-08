#pragma once

#include <cstdlib>
#include <random>
#include <sstream>

namespace lumina
{
    double getRandomFloat(float lower, float upper)
    {
        return lower + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(upper-lower)));
    }

    void relative2Pixel(float relative_x, float relative_y, int w, int h, int& x, int& y)
    {
        x = relative_x * w;
        y = relative_y * h;
    }
}

// TODO: Remove occurences in the other util files.
inline void loadGeometryFromFile(const std::string& filepath, std::vector<float>& vertices, unsigned int& num_vertices, std::vector<unsigned int>& indices, unsigned int& num_indices)
{
    num_vertices = 0;
    num_indices = 0;

    std::ifstream file(filepath);
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
        std::cerr << "Unable to open file: " << filepath << std::endl;
    }

    return;
}