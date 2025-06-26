#include <iostream>
#include "texture_factory.hpp"

TextureFactory::TextureFactory() {
    clear_cache();
}

TextureFactory::~TextureFactory() = default;

std::shared_ptr<Texture2D> TextureFactory::get_texture(
    const std::string& texture_path
) {
    const auto it = m_texture_cache.find(texture_path);

    if (it != m_texture_cache.end())
    {
        return it->second;
    }

    std::cerr << "Failed to retrieve texture: " << texture_path << "\n";

    return nullptr;
}

bool TextureFactory::load_texture(
    const std::string& texture_path,
    Texture2DConfig texture_config
) {
    auto texture_ptr = std::make_shared<Texture2D>();

    texture_ptr->load_from_file(
        texture_path,
        texture_config
    );
    
    m_texture_cache[texture_path] = texture_ptr;

    return true;
}

void TextureFactory::clear_cache() {
    m_texture_cache.clear();
}

