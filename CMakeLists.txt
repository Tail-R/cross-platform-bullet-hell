cmake_minimum_required(VERSION 3.26)
project(bullet_hell LANGUAGES C CXX)

# Target Name
set(TARGET_NAME bullet_hell)

# c++ version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# c version
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

# Source directories
set(SRC_DIR src)

# Source files
set(SRC_FILES
    # Entry point
    ${SRC_DIR}/main.cpp

    # Game system
    ${SRC_DIR}/packet_template/packet_template.cpp
    ${SRC_DIR}/packet_template/frame/frame.cpp
    ${SRC_DIR}/packet_serializer/header_serializer.cpp
    ${SRC_DIR}/packet_serializer/frame_serializer.cpp
    ${SRC_DIR}/packet_serializer/greeting_serializer.cpp
    ${SRC_DIR}/packet_serializer/game_serializer.cpp
    ${SRC_DIR}/packet_serializer/input_serializer.cpp
    ${SRC_DIR}/socket/socket.cpp
    ${SRC_DIR}/packet_stream/packet_stream.cpp
    ${SRC_DIR}/game_server/game_server.cpp

    # SDL2 abstract class
    ${SRC_DIR}/app/app.cpp
    ${SRC_DIR}/input_manager/input_manager.cpp

    # OpenGL abstract class
    ${SRC_DIR}/mesh/mesh.cpp
    ${SRC_DIR}/shader/shader.cpp
    ${SRC_DIR}/texture/texture2d.cpp
    ${SRC_DIR}/renderer/renderer.cpp
    ${SRC_DIR}/assets_factory/mesh_factory.cpp
    ${SRC_DIR}/assets_factory/shader_factory.cpp
    ${SRC_DIR}/assets_factory/texture_factory.cpp

    # Utilities
    ${SRC_DIR}/logger/logger.cpp

    # Glad
    external/glad/src/glad.c
)

# Include scripts
include(${CMAKE_SOURCE_DIR}/external/FetchSDL.cmake)
include(${CMAKE_SOURCE_DIR}/external/FetchSol2.cmake)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external")
find_package(LuaJIT REQUIRED)

# Executable
add_executable(${TARGET_NAME} ${SRC_FILES})

# Include paths
target_include_directories(${TARGET_NAME} PRIVATE
    src
    external/glad/include
    external/glm
)

# Link libraries
target_link_libraries(${TARGET_NAME} PRIVATE
    SDL2::SDL2
    luajit
    sol2::sol2
)

# Link OS-specific libraries
# Winsock2
if(WIN32)
    if (MINGW)
        target_link_libraries(${TARGET_NAME} PRIVATE mingw32)
    endif()

    target_link_libraries(${TARGET_NAME} PRIVATE ws2_32)
endif()

target_compile_definitions(${TARGET_NAME} PRIVATE
    PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}"
)
