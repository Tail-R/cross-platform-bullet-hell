#pragma once

#include <glad/glad.h>
#include <SDL2/SDL.h>

struct SDLConfig {
    int             gl_context_major_version    = 4;
    int             gl_context_minor_version    = 6;
    SDL_GLprofile   gl_context_profile          = SDL_GL_CONTEXT_PROFILE_CORE;
    std::string     window_name                 = "My first app! uwu";
    int             window_pos_x                = SDL_WINDOWPOS_UNDEFINED;
    int             window_pos_y                = SDL_WINDOWPOS_UNDEFINED;
    int             window_width                = 384;
    int             window_height               = 448;
    int             window_flags                = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;
    int             window_fps                  = 60;
};

enum class BlendMode {
    None,
    Alpha,
    Custom
};

enum class VSyncMode {
    None        = 0,
    Disable     = 0,
    Enable      = 1,
    Adaptive    = -1
};

struct GLConfig {
    BlendMode   blend_mode  = BlendMode::Alpha;
    VSyncMode   vsync_mode  = VSyncMode::Enable;

    // Optional for custom blending
    GLenum      custom_blend_src = GL_SRC_ALPHA;
    GLenum      custom_blend_dst = GL_ONE_MINUS_SRC_ALPHA;
};

struct AppResult {
    int         code            = 1;
    size_t      time_elapsed    = 0;
};

class App {
public:
    App();
    ~App();

    bool initialize(
        const SDLConfig& sdl_config = SDLConfig(),
        const GLConfig& gl_config = GLConfig()
    );

    // Delete copy constructor and copy assignment operator
    App(const App&) = delete;
    App& operator=(const App&) = delete;

    void cleanup();

    AppResult run();

private:
    SDL_Window*     m_sdl_window;
    SDL_GLContext   m_sdl_gl_context;

    bool            m_sdl_initialized;
    bool            m_sdl_gl_initialized;

    bool            init_sdl(const SDLConfig& app_config);
    bool            init_gl(const GLConfig& gl_config);

    void            cleanup_sdl();
    void            cleanup_gl();

    void            show_window();
    void            hide_window();
};