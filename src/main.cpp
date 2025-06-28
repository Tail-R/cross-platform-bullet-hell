#define SDL_MAIN_HANDLED

#include <iostream>
#include "app/app.hpp"

int main(int argc, char* args[]) {
    // Unused
    static_cast<void>(argc);
    static_cast<void>(args);

    App             app;
    SDLConfig       sdl_config;
    SDL_DisplayMode disp_mode;

    SDL_GetCurrentDisplayMode(0, &disp_mode);

    sdl_config.window_width = disp_mode.w;
    sdl_config.window_height = disp_mode.h;
    sdl_config.window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

    if (!app.initialize(sdl_config))
    {
        std::cerr << "Failed to initialize application" << "\n";

        return EXIT_FAILURE;
    }

    auto app_result = app.run();

    return app_result.code;
}