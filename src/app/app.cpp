#include <iostream>
#include <string>
#include "app.hpp"
#include "../config_constants.hpp"
#include "../logger/logger.hpp"
#include "../input_manager/input_manager.hpp"

#include "../assets_factory/mesh_factory.hpp"
#include "../assets_factory/shader_factory.hpp"
#include "../assets_factory/texture_factory.hpp"

#include "../packet_stream/packet_stream.hpp"
#include "../game_server/game_server.hpp"

App::App()
    : m_sdl_window(nullptr)
    , m_sdl_gl_context(nullptr)
    , m_sdl_initialized(false)
    , m_sdl_gl_initialized(false)
{
    std::cout << "[App] DEBUG: App has been started" << "\n";

    start_async_logger(logger_constants::LOG_FILE_NAME.data());
}

App::~App() {
    cleanup();

    stop_async_logger();

    std::cout << "[App] DEBUG: App shutdown complete" << "\n";
}

AppResult App::run() {
    AppResult app_result;
    InputManager input_manager;

    auto client_socket = std::make_shared<ClientSocket>(
        socket_constants::SERVER_ADDR,
        socket_constants::SERVER_PORT
    );

    if (!client_socket->connect_to_server())
    {
        std::cerr << "[App] DEBUG: Failed to connect to server" << "\n";

        return app_result;
    }

    PacketStreamClient packet_stream(client_socket);
    packet_stream.start();

    // Set viewport as display size
    // SDL_DisplayMode disp_mode;
    // SDL_GetCurrentDisplayMode(0, &disp_mode);
    // glViewport(0, 0, disp_mode.w, disp_mode.h);

    bool quit = false;
 
    auto mf = MeshFactory();
    auto sf = ShaderFactory();
    auto tf = TextureFactory();

    const auto mesh_path        = std::string(assets_constants::MESH_DIR)       + "/square.lua";
    const auto bg_mesh_path     = std::string(assets_constants::MESH_DIR)       + "/full_size.lua";
    const auto shader_path      = std::string(assets_constants::SHADER_DIR)     + "/green_aura.lua";
    const auto bg_shader_path   = std::string(assets_constants::SHADER_DIR)     + "/basic.lua";
    const auto texture_path     = std::string(assets_constants::TEXTURE_DIR)    + "/zunmon_3002.png";
    const auto bg_texture_path  = std::string(assets_constants::TEXTURE_DIR)    + "/background109.png"; 

    mf.load_mesh(mesh_path);
    mf.load_mesh(bg_mesh_path);
    sf.load_shader(shader_path);
    sf.load_shader(bg_shader_path);
    tf.load_texture(texture_path);
    tf.load_texture(bg_texture_path);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    float x_offset = 0.0;
    float y_offset = 0.0;
    float speed = 2.0f;
    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 last = 0;
    float total_time = 0.0;

    show_window();

    while (!quit)
    {
        last = now;
        now = SDL_GetPerformanceCounter();
        float delta_time = (now - last) / (float)SDL_GetPerformanceFrequency();
        total_time += delta_time;

        glClear(GL_COLOR_BUFFER_BIT);

        input_manager.collect_input_events();
        
        if (input_manager.get_quit_request())
        {
            quit = true;
        }

        auto game_input = input_manager.get_game_input();

        if (game_input.pressed.test(static_cast<size_t>(GameAction::Shoot)))
        {
            quit = true;

            std::cout << "[App] DEBUG: Quit has been pressed" << "\n"
                      << "[App] DEBUG: Exiting the draw loop" << "\n";
        } 

        // Send client input
        if (game_input.arrows.pressed.any() || game_input.arrows.released.any())
        {
            ClientInput input;
            input.game_input = game_input;

            const auto packet = make_packet(input);
            const auto send_result = packet_stream.send_packet(packet);

            if (!send_result)
            {
                std::cerr << "[App] ERROR: Failed to send client input" << "\n";
            }
        }

        // Get frame
        auto frame_opt = packet_stream.poll_frame();

        if (frame_opt.has_value())
        {
            x_offset = frame_opt.value().player_vector[0].pos.x;
            y_offset = frame_opt.value().player_vector[0].pos.y;
        }

        // Background
        auto mesh = mf.get_mesh(bg_mesh_path);
        auto shader = sf.get_shader(bg_shader_path);
        auto texture = tf.get_texture(bg_texture_path);

        shader->use();
        shader->set_float("time", total_time);
        shader->set_vec2("offset", glm::vec2(0.0f, 0.0f));

        texture->bind();
        mesh->bind();
        mesh->draw();

        // Player
        mesh = mf.get_mesh(mesh_path);
        shader = sf.get_shader(shader_path);
        texture = tf.get_texture(texture_path);

        shader->use();
        shader->set_float("time", total_time);
        shader->set_vec2("offset", glm::vec2(x_offset, y_offset));

        texture->bind();
        mesh->bind();
        mesh->draw();

        SDL_GL_SwapWindow(m_sdl_window);
    }

    // game_server_master->stop();
    packet_stream.stop();
    client_socket->disconnect();

    hide_window();

    return app_result;
}

bool App::initialize(const SDLConfig& sdl_config, const GLConfig& gl_config) {
    m_sdl_initialized = init_sdl(sdl_config);
    m_sdl_gl_initialized = init_gl(gl_config);

    return m_sdl_initialized && m_sdl_gl_initialized;
}

bool App::init_sdl(const SDLConfig& sdl_config) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, sdl_config.gl_context_major_version);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, sdl_config.gl_context_minor_version);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, sdl_config.gl_context_profile);
    
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        auto err_msg = std::string("SDL could not initialize! SDL_Error: ") + SDL_GetError();
        async_log(LogLevel::Error, err_msg);
        cleanup_sdl();

        return false;
    }

    // Create window
    m_sdl_window = SDL_CreateWindow(
        sdl_config.window_name.data(),
        sdl_config.window_pos_x,
        sdl_config.window_pos_y,
        sdl_config.window_width,
        sdl_config.window_height,
        sdl_config.window_flags
    );

    if (m_sdl_window == nullptr)
    {
        auto err_msg = std::string("Window could not be created! SDL_Error: ") + SDL_GetError();
        async_log(LogLevel::Error, err_msg);
        cleanup_sdl();

        return false;
    }

    return true;
}

bool App::init_gl(const GLConfig& gl_config) {
    // Create OpenGL context
    m_sdl_gl_context = SDL_GL_CreateContext(m_sdl_window);

    if (m_sdl_gl_context == nullptr)
    {
        async_log(LogLevel::Error, "OpenGL context could not be created!");
        cleanup_gl();

        return false;
    }

    // Load function pointers
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        async_log(LogLevel::Error, "Failed to initialize GLAD");
        cleanup_gl();

        return false;
    }

    // Set blend mode
    if (gl_config.blend_mode != BlendMode::None)
    {
        glEnable(GL_BLEND);

        switch (gl_config.blend_mode)
        {
            case BlendMode::Alpha:
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                break;
            
            case BlendMode::Custom:
                glBlendFunc(gl_config.custom_blend_src, gl_config.custom_blend_dst);

                break;

            default:
                glDisable(GL_BLEND);
                async_log(LogLevel::Warning, "Unknown blend mode");
        }
    }
    else
    {
        glDisable(GL_BLEND);
    }

    /*
        1 = VSync ON (Synchronize to display refresh rate)
        0 = VSync OFF (Immediate update)
        -1 = Adaptive VSync (Enable if its supported)
    */
    auto interval = static_cast<int>(gl_config.vsync_mode);
    switch (interval)
    {
        case 0: case 1: case -1:
            SDL_GL_SetSwapInterval(interval);

            break;

        default:
            SDL_GL_SetSwapInterval(0);
            async_log(LogLevel::Warning, "Unknown vsync mode");
    }

    return true;
}

void App::cleanup() {
    cleanup_gl();
    cleanup_sdl();
}

void App::cleanup_sdl() {
    if (m_sdl_window != nullptr)
    {
        SDL_DestroyWindow(m_sdl_window);
        m_sdl_window = nullptr;
    }

    if (m_sdl_initialized)
    {
        SDL_Quit();
        m_sdl_initialized = false;
    }
}

void App::cleanup_gl() {
    if (m_sdl_gl_context != nullptr)
    {
        SDL_GL_DeleteContext(m_sdl_gl_context);
        m_sdl_gl_context = nullptr;

        m_sdl_gl_initialized = false;
    }
}

void App::show_window() {
    SDL_ShowWindow(m_sdl_window);
}

void App::hide_window() {
    SDL_HideWindow(m_sdl_window);
}