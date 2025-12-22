#pragma once

#include <memory>
#include <vector>
#include <stdexcept>
#include <fstream>

#include "lumina/lumina.hpp"

namespace lumina {
namespace ui
{
class Factory
{
public:
    // Dependency injection maps
    std::unordered_map<std::string, std::shared_ptr<lumina::Shader>> shaders;
    std::unordered_map<std::string, std::shared_ptr<lumina::Mesh>> meshes;
    std::unordered_map<std::string, glm::vec4> colors;
    std::shared_ptr<lumina::Font> font;
    lumina::ui::Context& context;  // Injected UI context

    Factory(
        const std::unordered_map<std::string, std::shared_ptr<lumina::Shader>>& shaders_,
        const std::unordered_map<std::string, std::shared_ptr<lumina::Mesh>>& meshes_,
        const std::unordered_map<std::string, glm::vec4>& colors_,
        std::shared_ptr<lumina::Font> font_,
        lumina::ui::Context& context_)
        : shaders(shaders_), meshes(meshes_), colors(colors_), font(font_), context(context_)
    {}

    std::shared_ptr<lumina::ui::Element> create(const nlohmann::json& j)
    {
        const std::string type = j.at("type").get<std::string>();
        const std::string id   = j.at("id").get<std::string>();

        auto rect = j.value("rect", std::vector<float>{0.0f, 0.0f, 0.0f, 0.0f});

        if (type == "Canvas")
        {
            auto canvas = std::make_shared<lumina::ui::Canvas>(
                id,
                rect[0], rect[1], rect[2], rect[3],
                shaders.at("ui"),
                meshes.at("quad"),
                colors.at(j.value("color", "white"))
            );

            addChildren(canvas, j);
            return canvas;
        }

        if (type == "Button")
        {
            auto button = std::make_shared<lumina::ui::Button>(
                id,
                rect[0], rect[1], rect[2], rect[3],
                j.value("text", ""),
                context, // Use injected context
                colors.at(j.value("textColor", "white")),
                colors.at(j.value("bgColor", "black")),
                colors.at(j.value("hoverColor", "white")),
                colors.at(j.value("pressedColor", "black"))
            );

            if (j.contains("onClick"))
                button->setOnClick(j["onClick"]);

            return button;
        }

        if (type == "VerticalLayout")
        {
            auto layout = std::make_shared<lumina::ui::VerticalLayout>(
                id,
                rect[0], rect[1], rect[2], rect[3],
                j.value("spacing", 0.0f)
            );

            addChildren(layout, j);
            layout->updateChildDimensions();
            return layout;
        }

        if (type == "HorizontalLayout")
        {
            auto layout = std::make_shared<lumina::ui::HorizontalLayout>(
                id,
                rect[0], rect[1], rect[2], rect[3],
                j.value("spacing", 0.0f)
            );

            addChildren(layout, j);
            layout->updateChildDimensions();
            return layout;
        }

        if (type == "List")
        {
            auto list = std::make_shared<lumina::ui::List>(
                id,
                rect[0], rect[1], rect[2], rect[3],
                shaders.at("ui"),
                font,
                meshes.at("quad")
            );

            addChildren(list, j);
            list->updateChildDimensions();
            return list;
        }

        throw std::runtime_error("UIFactory: Unknown UI type: " + type);
    }

private:
    void addChildren(std::shared_ptr<lumina::ui::Element> parent, const nlohmann::json& j)
    {
        if (!j.contains("children")) return;

        for (const auto& child : j["children"])
        {
            auto element = create(child);
            parent->addChild(element);
        }
    }
};
}
}

