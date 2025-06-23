#include <string>
#include "app.hpp"
#include "../config_constants.hpp"
#include "../logger/logger.hpp"

App::App()
    : m_sdl_window(nullptr)
    , m_sdl_gl_context(nullptr)
    , m_sdl_initialized(false)
    , m_sdl_gl_initialized(false)
{
    start_async_logger(logger_constants::LOG_FILE_NAME.data());
    async_log(LogLevel::Info, "App has been started");
}

App::~App() {
    cleanup();

    async_log(LogLevel::Info, "App has been cleanup");
    stop_async_logger();
}

bool App::initialize(AppConfig app_config) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, app_config.gl_context_major_version);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, app_config.gl_context_minor_version);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, app_config.gl_context_profile);
    
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        auto err_msg = std::string("SDL could not initialize! SDL_Error: ") + SDL_GetError() + "\n";
        async_log(LogLevel::Error, err_msg);

        return false;
    }

    // Create window
    m_sdl_window = SDL_CreateWindow(
        app_config.window_name.data(),
        app_config.window_pos_x,
        app_config.window_pos_y,
        app_config.window_width,
        app_config.window_height,
        app_config.window_flags
    );

    if (m_sdl_window == nullptr)
    {
        auto err_msg = std::string("Window could not be created! SDL_Error: ") + SDL_GetError() + "\n";
        async_log(LogLevel::Error, err_msg);

        return false;
    }

    // Create OpenGL context
    m_sdl_gl_context = SDL_GL_CreateContext(m_sdl_window);

    if (m_sdl_gl_context == nullptr)
    {
        auto err_msg = "OpenGL context could not be created!\n";
        async_log(LogLevel::Error, err_msg);

        return false;
    }

    return true;
}

void App::cleanup() {
    if (m_sdl_window != nullptr)
    {
        SDL_DestroyWindow(m_sdl_window);
        m_sdl_window = nullptr;
    }

    if (m_sdl_gl_context != nullptr)
    {
        SDL_GL_DeleteContext(m_sdl_gl_context);
        m_sdl_gl_initialized = false;
    }

    if (m_sdl_initialized)
    {
        SDL_Quit();
        m_sdl_initialized = false;
    }
}

AppResult App::run() {
    AppResult app_result;

    return app_result;
}