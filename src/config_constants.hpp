#pragma once

#define ENABLE_LOCAL_SERVER
#define BUILD_CLIENT
// #define BUILD_SERVER

#include <cstdint>
#include <string_view>

/*
    The variable PROJECT_ROOT_DIR is a macro declared in CMakeLists.txt
    No need to worry about errors that your editor'sIntelliSense shows you
*/

namespace general_constants {
    constexpr std::string_view  ASSETS_DIR      = PROJECT_ROOT_DIR "/assets";
}

namespace assets_constants {
    constexpr std::string_view  CHARACTER_DIR   = PROJECT_ROOT_DIR "/assets/character";
    constexpr std::string_view  MESH_DIR        = PROJECT_ROOT_DIR "/assets/mesh";
    constexpr std::string_view  SHADER_DIR      = PROJECT_ROOT_DIR "/assets/shader";
    constexpr std::string_view  TEXTURE_DIR     = PROJECT_ROOT_DIR "/assets/texture";
}

namespace logger_constants {
    constexpr std::string_view  LOG_FILE_DIR    = PROJECT_ROOT_DIR "log";
    constexpr std::string_view  LOG_FILE_NAME   = "app.log";
}

namespace render_constants {
    constexpr std::string_view  WINDOW_NAME     = "bullet_hell";
    constexpr size_t            WINDOW_WIDTH    = 600;
    constexpr size_t            WINDOW_HEIGHT   = 800;
}

namespace socket_constants {
#ifdef BUILD_CLIENT
    #ifdef ENABLE_LOCAL_SERVER
        constexpr std::string_view  SERVER_ADDR             = "127.0.0.1";
        constexpr uint16_t          SERVER_PORT             = 2222;
        constexpr size_t            SERVER_MAX_INSTANCES    = 1;
    #else
        constexpr std::string_view  SERVER_ADDR             = "150.42.11.6";
        constexpr uint16_t          SERVER_PORT             = 6198;
    #endif

#elif defined(BUILD_SERVER)
    constexpr std::string_view      SERVER_ADDR             = "127.0.0.1";
    constexpr uint16_t              SERVER_PORT             = 22222;
    constexpr size_t                SERVER_MAX_INSTANCES    = 10;
#endif

    constexpr uint32_t  SERVER_MAX_PACKET_SIZE  = 10 * 1024 * 1024; // 10MB
}