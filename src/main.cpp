#define SDL_MAIN_HANDLED

#include <iostream>
#include "app/app.hpp"

int main(int argc, char* args[]) {
    // Unused
    static_cast<void>(argc);
    static_cast<void>(args);

    App         app;
    SDLConfig   sdl_config;
    GLConfig    gl_config;

    sdl_config.window_width     = 400;
    sdl_config.window_height    = 600;

    if (!app.initialize(sdl_config, gl_config))
    {
        std::cerr << "Failed to initialize application" << "\n";

        return EXIT_FAILURE;
    }

    auto app_result = app.run();

    return app_result.code;
}