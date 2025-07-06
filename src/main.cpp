#define SDL_MAIN_HANDLED

#include <iostream>
#include "config_constants.hpp"
#include "app/app.hpp"
#include "game_server/game_server.hpp"

int main(int argc, char* args[]) {
    // Unused
    static_cast<void>(argc);
    static_cast<void>(args);

#ifdef BUILD_CLIENT
    #ifdef ENABLE_LOCAL_SERVER
        auto game_server_master = std::make_shared<GameServerMaster>(
            socket_constants::SERVER_PORT,
            socket_constants::SERVER_MAX_INSTANCES
        );

        if (!game_server_master->initialize())
        {
            std::cerr << "[App] Failed to initialize game server master" << "\n";
        }
        else
        {
            std::cout << "[App] Game server master has been initialized" << "\n";
        }

        game_server_master->run_async();
        game_server_master->wait_for_accept_ready(1000, 10);
    #endif

    App             app;
    SDLConfig       sdl_config;
    SDL_DisplayMode disp_mode;

    SDL_GetCurrentDisplayMode(0, &disp_mode);

    sdl_config.window_width  = disp_mode.w;
    sdl_config.window_height = disp_mode.h;
    sdl_config.window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

    if (!app.initialize(sdl_config))
    {
        std::cerr << "Failed to initialize application" << "\n";

        return EXIT_FAILURE;
    }

    auto app_result = app.run();

    return app_result.code;

#elif defined(BUILD_SERVER)
    auto game_server_master = std::make_shared<GameServerMaster>(
        socket_constants::SERVER_PORT,
        socket_constants::SERVER_MAX_INSTANCES
    );

    if (!game_server_master->initialize())
    {
        std::cerr << "[App] Failed to initialize game server master" << "\n";
    }
    else
    {
        std::cout <<"[App] Game server master has been initialized" << "\n";
    }

    game_server_master->run();
#endif
}