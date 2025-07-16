#include <iostream>
#include "texture_factory.hpp"

TextureFactory::TextureFactory() = default;
TextureFactory::~TextureFactory() = default;

std::shared_ptr<Texture2D> TextureFactory::get_texture(
    const std::string& texture_path
) {
    const auto it = m_texture_cache.find(texture_path);

    if (it != m_texture_cache.end())
    {
        return it->second;
    }

    std::cerr << "[TextureFactory] WARNING: Texture not preloaded " << texture_path << "\n";

    // Fallback Texture created
    auto default_texture = std::make_shared<Texture2D>();
    default_texture->load_default_texture();

    m_texture_cache[texture_path] = default_texture;

    return default_texture;
}

void TextureFactory::load_texture(
    const std::string& texture_path,
    Texture2DConfig texture_config
) {
    if (m_texture_cache.find(texture_path) != m_texture_cache.end())
    {
        return;
    }

    auto texture_ptr = std::make_shared<Texture2D>();

    texture_ptr->load_from_file(
        texture_path,
        texture_config
    );
    
    m_texture_cache[texture_path] = texture_ptr;
}

void TextureFactory::clear_cache() {
    m_texture_cache.clear();
}

