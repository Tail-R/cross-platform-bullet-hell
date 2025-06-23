#define SDL_MAIN_HANDLED

#include <iostream>
#include "app/app.hpp"

int main(int argc, char* args[]) {
    // Unused
    static_cast<void>(argc);
    static_cast<void>(args);

    AppConfig   config;
    App         app;

    if (!app.initialize(config))
    {
        std::cerr << "Failed to initialize application" << "\n";

        return EXIT_FAILURE;
    }

    auto app_result = app.run();

    return app_result.code;
}