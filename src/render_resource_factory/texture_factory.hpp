#pragma once

#include <string>
#include <memory>
#include <optional>
#include <unordered_map>
#include "../texture/texture2d.hpp"

class TextureFactory {
public:
    TextureFactory();
    ~TextureFactory();

    std::optional<std::shared_ptr<Texture2D>> get_texture(
        const std::string& texture_path
    );

    bool load_texture(
        const std::string& texture_path,
        Texture2DConfig texture_config = Texture2DConfig()
    );

    void clear_cache();

private:
    std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_texture_cache;
};