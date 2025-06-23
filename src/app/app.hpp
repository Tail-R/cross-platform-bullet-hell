#pragma once

#include "SDL2/SDL.h"

struct AppConfig {
    int             gl_context_major_version    = 4;
    int             gl_context_minor_version    = 6;
    SDL_GLprofile   gl_context_profile          = SDL_GL_CONTEXT_PROFILE_CORE;
    std::string     window_name                 = "My first app! uwu";
    int             window_pos_x                = SDL_WINDOWPOS_UNDEFINED;
    int             window_pos_y                = SDL_WINDOWPOS_UNDEFINED;
    int             window_width                = 600;
    int             window_height               = 400;
    int             window_flags                = SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL;
};

struct AppResult {
    int code = 1;
    size_t time_elapsed = 0;
};

class App {
public:
    App();
    ~App();

    bool initialize(AppConfig app_config);

    void cleanup();
    AppResult run();

private:
    SDL_Window*     m_sdl_window;
    SDL_GLContext   m_sdl_gl_context;

    bool            m_sdl_initialized;
    bool            m_sdl_gl_initialized;
};