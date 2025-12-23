#pragma once

#include <memory>
#include <string>
#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>

#include "lumina/ui/element.hpp"
#include "lumina/ui/factory.hpp"

namespace lumina::ui
{

class Loader
{
public:
    explicit Loader(std::shared_ptr<Factory> factory)
        : factory_(std::move(factory))
    {}

    std::shared_ptr<Element> loadFromFile(const std::string& path)
    {
        std::ifstream file(path);
        if (!file.is_open())
        {
            std::cerr << "[UI::Loader] Failed to open file: "
                      << path << std::endl;
            return nullptr;
        }

        try
        {
            nlohmann::json j;
            file >> j;
            return loadFromJson(j);
        }
        catch (const std::exception& e)
        {
            std::cerr << "[UI::Loader] JSON error in "
                      << path << ": " << e.what() << std::endl;
            return nullptr;
        }
    }

    std::shared_ptr<Element> loadFromJson(const nlohmann::json& j)
    {
        try
        {
            return factory_->create(j);
        }
        catch (const std::exception& e)
        {
            std::cerr << "[UI::Loader] Factory error: "
                      << e.what() << std::endl;
            return nullptr;
        }
    }

private:
    std::shared_ptr<Factory> factory_;
};

}
