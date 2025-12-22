#pragma once


#include "utils.hpp"
#include "../mesh.hpp"
#include "../shader.hpp"
#include "../font.hpp"

namespace lumina {
namespace ui
{
    struct Context
    {
        std::shared_ptr<::lumina::Shader> shader_ptr;
        std::shared_ptr<::lumina::Font> font_ptr;
        std::shared_ptr<::lumina::Mesh> mesh_ptr;
    };
}
}