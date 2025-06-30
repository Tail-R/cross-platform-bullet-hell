cmake_minimum_required(VERSION 3.26)
project(bullet_hell_client LANGUAGES C CXX)

# Target Name
set(TARGET_NAME bullet_hell_client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

# Source directories
set(SRC_DIR src)
set(GLAD_SRC external/glad/src/glad.c)

# Source files
set(SRC_FILES
    # Entry point
    ${SRC_DIR}/main.cpp

    # Game system
    ${SRC_DIR}/packet_template/frame/frame.cpp
    ${SRC_DIR}/packet_serializer/header_serializer.cpp
    ${SRC_DIR}/packet_serializer/frame_serializer.cpp
    ${SRC_DIR}/packet_serializer/greeting_serializer.cpp
    ${SRC_DIR}/packet_serializer/game_serializer.cpp
    ${SRC_DIR}/packet_serializer/input_serializer.cpp
    ${SRC_DIR}/socket/socket.cpp
    ${SRC_DIR}/packet_stream/packet_stream.cpp

    # SDL2 abstract class
    ${SRC_DIR}/app/app.cpp
    ${SRC_DIR}/input_manager/input_manager.cpp

    # OpenGl abstract class
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
    ${GLAD_SRC}
)

# Include fetch scripts
include(${CMAKE_SOURCE_DIR}/external/FetchSDL.cmake)
include(${CMAKE_SOURCE_DIR}/external/FetchSol2.cmake)
include(${CMAKE_SOURCE_DIR}/external/ImportLuaJIT.cmake)

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
if(WIN32)
    target_link_libraries(${TARGET_NAME} PRIVATE
        mingw32
        ws2_32
    )
elseif(UNIX)

endif()

add_compile_definitions(PROJECT_ROOT_DIR=\"${CMAKE_SOURCE_DIR}\")
