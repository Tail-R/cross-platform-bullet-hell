#include "texture_factory.hpp"

TextureFactory::TextureFactory() {
    clear_cache();
}

TextureFactory::~TextureFactory() {

}

std::optional<std::shared_ptr<Texture2D>> get_texture(
    const std::string& texture_path
) {
    // Implement soon

    return std::nullopt;
}

bool TextureFactory::load_texture(
    const std::string& texture_path,
    Texture2DConfig texture_config
) {
    auto texture = Texture2D();

    const auto load_result = texture.load_from_file(
        texture_path,
        texture_config
    );

    m_texture_cache[texture_path] = std::make_shared<Texture2D>(
        std::move(texture)
    );

    return load_result;
}

void TextureFactory::clear_cache() {
    m_texture_cache.clear();
}

