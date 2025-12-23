#pragma once

#include <filesystem>
#include <unordered_map>
#include <memory>
#include <string>
#include <iostream>

#include "lumina/ui/system.hpp"
#include "lumina/ui/element.hpp"
#include "lumina/ui/loader.hpp"

namespace lumina::ui
{

class Watcher
{
public:
    Watcher(System& ui, Loader& loader)
        : ui_(ui), loader_(loader)
    {}

    bool reloadIfChanged(const std::string& path,
                         std::shared_ptr<Element>& target,
                         bool default_active = false)
    {
        namespace fs = std::filesystem;

        if (!fs::exists(path))
        {
            std::cerr << "[UI::Watcher] File does not exist: "
                      << path << std::endl;
            return false;
        }

        fs::file_time_type current_time = fs::last_write_time(path);

        auto it = timestamps_.find(path);
        if (it != timestamps_.end() && it->second == current_time)
            return false;

        bool was_active = target
            ? target->isActive()
            : default_active;

        std::shared_ptr<Element> new_ui =
            loader_.loadFromFile(path);

        if (!new_ui)
            return false;

        if (target)
        {
            ui_.remove(target);
        }

        new_ui->setActive(was_active);
        ui_.add(new_ui);

        target = new_ui;
        timestamps_[path] = current_time;

        return true;
    }

    void clear()
    {
        timestamps_.clear();
    }

private:
    System& ui_;
    Loader& loader_;

    std::unordered_map<
        std::string,
        std::filesystem::file_time_type
    > timestamps_;
};

}
