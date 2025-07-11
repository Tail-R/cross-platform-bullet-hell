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

App::App()
    : m_sdl_window(nullptr)
    , m_sdl_gl_context(nullptr)
    , m_sdl_initialized(false)
    , m_sdl_gl_initialized(false)
{
    start_async_logger(logger_constants::LOG_FILE_NAME.data());
    async_log(LogLevel::Debug, "App has been started");
}

App::~App() {
    cleanup();

    async_log(LogLevel::Debug, "App shutdown complete");
    stop_async_logger();
}

AppResult App::run() {
    InputManager input_manager;

    auto client_socket = std::make_shared<ClientSocket>(
        socket_constants::SERVER_ADDR,
        socket_constants::SERVER_PORT
    );

    if (!client_socket->connect_to_server())
    {
        async_log(LogLevel::Error, "Failed to connect to server");

        return AppResult {
            AppExitStatus::SocketError
        };
    }

    show_window();

    PacketStreamClient packet_stream(client_socket);
    packet_stream.start();

    // A closure that waits for a specific packet to arrive.
    auto wait_packet = [&](PayloadType payload_type, size_t timeout_msec, size_t max_attempts) -> bool {
        for (size_t attempt = 0; attempt < max_attempts; attempt++)
        {
            // Check if the recv thread is alive
            const auto expr_1 = packet_stream.get_recv_exception() == nullptr;
            const auto expr_2 = packet_stream.is_running();

            if (!expr_1 || !expr_2)
            {
                return false;
            }

            std::optional<Packet> packet_opt = packet_stream.poll_packet();

            if (packet_opt.has_value())
            {
                if (packet_opt.value().header.payload_type == payload_type)
                {
                    return true;
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(timeout_msec));
        }

        return false;
    };

    // Send client hello
    packet_stream.send_packet(make_packet<ClientHello>({}));
    async_log(LogLevel::Debug, "Client hello has been sent");

    // Wait for server accept
    if (!wait_packet(PayloadType::ServerAccept, 1000, 10))
    {
        async_log(LogLevel::Error, "Server accept timeout");

        return AppResult {
            AppExitStatus::ServerTimeout
        };
    }

    // Send game request
    packet_stream.send_packet(make_packet<ClientGameRequest>({}));
    async_log(LogLevel::Debug, "Client game request has been sent");
 
    // Factories
    auto mesh_factory = MeshFactory();
    auto shader_factory = ShaderFactory();
    auto texture_factory = TextureFactory();

    // Player assets
    const auto p_mesh_path      = std::string(assets_constants::MESH_DIR)       + "/square.lua";
    const auto p_shader_path    = std::string(assets_constants::SHADER_DIR)     + "/green_aura.lua";
    const auto p_texture_path   = std::string(assets_constants::TEXTURE_DIR)    + "/zunmon_3002.png";

    // Background assets
    const auto bg_mesh_path     = std::string(assets_constants::MESH_DIR)       + "/full_size.lua";
    const auto bg_shader_path   = std::string(assets_constants::SHADER_DIR)     + "/basic.lua";
    const auto bg_texture_path  = std::string(assets_constants::TEXTURE_DIR)    + "/background109.png"; 

    // Load player assets
    mesh_factory.load_mesh(p_mesh_path);
    shader_factory.load_shader(p_shader_path);
    texture_factory.load_texture(p_texture_path);

    // Load background assets
    mesh_factory.load_mesh(bg_mesh_path);
    shader_factory.load_shader(bg_shader_path);
    texture_factory.load_texture(bg_texture_path);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    bool quit = false;
    float x_offset = 0.0;
    float y_offset = 0.0;
    float speed = 2.0f;
    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 last = 0;
    float total_time = 0.0;

    // Wait for server game response
    if (!wait_packet(PayloadType::ServerGameResponse, 1000, 10))
    {
        async_log(LogLevel::Error, "Server game response timeout");

        return AppResult {
            AppExitStatus::ServerTimeout
        };
    }

    while (!quit)
    {
        last = now;
        now = SDL_GetPerformanceCounter();
        float delta_time = (now - last) / (float)SDL_GetPerformanceFrequency();
        total_time += delta_time;

        glClear(GL_COLOR_BUFFER_BIT);

        input_manager.collect_input_events();
        const auto game_input = input_manager.get_game_input();

        // Quit events
        const auto expr_1 = input_manager.get_quit_request();
        const auto expr_2 = game_input.pressed.test(static_cast<size_t>(GameAction::OpenMenu));
        
        if (expr_1 || expr_2)
        {
            // Send client goodbye
            packet_stream.send_packet(make_packet<ClientGoodbye>({}));
            async_log(LogLevel::Debug, "Client goodbye has been sent");
            async_log(LogLevel::Debug, "Quit has been pressed");
            async_log(LogLevel::Debug, "Exiting the draw loop");

            quit = true;
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
                async_log(LogLevel::Error, "Failed to send client input");
            }
        }

        // Get frame
        auto frame_opt = packet_stream.poll_frame();

        if (frame_opt.has_value())
        {
            x_offset = frame_opt.value().player_vector[0].pos.x;
            y_offset = frame_opt.value().player_vector[0].pos.y;
        }

        // Draw background
        auto mesh = mesh_factory.get_mesh(bg_mesh_path);
        auto shader = shader_factory.get_shader(bg_shader_path);
        auto texture = texture_factory.get_texture(bg_texture_path);

        shader->use();
        shader->set_float("time", total_time);
        shader->set_vec2("offset", glm::vec2(0.0f, 0.0f));

        texture->bind();
        mesh->bind();
        mesh->draw();

        // Draw player
        mesh = mesh_factory.get_mesh(p_mesh_path);
        shader = shader_factory.get_shader(p_shader_path);
        texture = texture_factory.get_texture(p_texture_path);

        shader->use();
        shader->set_float("time", total_time);
        shader->set_vec2("offset", glm::vec2(x_offset, y_offset));

        texture->bind();
        mesh->bind();
        mesh->draw();

        SDL_GL_SwapWindow(m_sdl_window);
    }

    // Wait for server goodbye
    if (!wait_packet(PayloadType::ServerGoodbye, 1000, 10))
    {
        async_log(LogLevel::Error, "Server goodbye timeout");

        return AppResult {
            AppExitStatus::ServerTimeout
        };
    }

    packet_stream.stop();
    client_socket->disconnect();

    return AppResult {
        AppExitStatus::Success
    };
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