#pragma once

#include <string>
#include <unordered_map>
#include "sprite_tag.hpp"

struct SpriteDimensions {
    std::string mesh;
    int         width;
    int         height;
};

struct SpriteTextureRegion {
    float u0;
    float v0;
    float u1;
    float v1;
};

struct SpriteTextureAtlas {
    std::string image;
    std::unordered_map<std::string, SpriteTextureRegion> regions;
};

struct SpriteAnimation {
    std::string     shader;
    std::string     region;
    float           duration;
    bool            loop;
};

struct Sprite {
    SpriteType          type;
    SpriteName          name;
    SpriteDimensions    dimensions;
    SpriteTextureAtlas  texture_atlas;
    std::unordered_map<std::string, SpriteAnimation> animations;
};
