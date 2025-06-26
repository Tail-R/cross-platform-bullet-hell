add_library(luajit STATIC IMPORTED GLOBAL)

set_target_properties(luajit PROPERTIES
    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/external/LuaJIT/src/libluajit.a"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/external/LuaJIT/src"
)
