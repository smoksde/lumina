#pragma once

#include <functional>
#include <string>
#include <unordered_map>

namespace lumina {
    namespace ui {
        using action = std::function<void()>;

        class ActionRegistry {
        public:
            std::unordered_map<std::string, action> actions;

            void registerAction(const std::string& name, action fn) {
                actions[name] = fn;
            }

            action get(const std::string& name) {
                auto it = actions.find(name);
                if (it != actions.end()) return it->second;
                return []() {};
            }

            static ActionRegistry& instance() {
                static ActionRegistry registry;
                return registry;
            }
        };
    }
}