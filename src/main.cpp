#define SDL_MAIN_HANDLED

#include <iostream>
#include "app/app.hpp"

int main(int argc, char* args[]) {
    // Unused
    static_cast<void>(argc);
    static_cast<void>(args);

    SDLConfig   sdl_config;
    GLConfig    gl_config;

    App         app;

    if (!app.initialize(sdl_config, gl_config))
    {
        std::cerr << "Failed to initialize application" << "\n";

        return EXIT_FAILURE;
    }

    auto app_result = app.run();

    return app_result.code;
}