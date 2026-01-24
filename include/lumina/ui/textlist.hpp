#pragma once

#include <vector>
#include <string>
#include <sstream>

#include "glm/glm.hpp"
#include "lumina/font.hpp"


namespace lumina::ui
{
    class TextList
    {
        public:
            TextList() = default;
            ~TextList() = default;

            void clear()
            {
                lines.clear();
            }

            void addLine(const std::string& line)
            {
                lines.push_back(line);
            }

            void addMultiline(const std::string& text)
            {
                std::istringstream ss(text);
                std::string line;
                while (std::getline(ss, line))
                {
                    lines.push_back(line);
                }
            }

            void render(lumina::Font& font)
            {
                float current_y = y;
                for (const auto& line : lines)
                {
                    font.drawString(x, current_y, line.c_str(), color);
                    current_y += line_height;
                }
            }

            void setPosition(float _x, float _y)
            {
                x = static_cast<float>(_x);
                y = static_cast<float>(_y);
            }

            void setColor(const glm::vec4& _color)
            {
                color = _color;
            }

            void setLineHeight(float _line_height)
            {
                line_height = _line_height;
            }

            std::vector<std::string>& getLines()
            {
                return lines;
            }

        private:

            float x;
            float y;
            float line_height = 40.0f;
            glm::vec4 color;
            std::vector<std::string> lines;
    };
};
