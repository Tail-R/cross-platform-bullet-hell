#include <iostream>
#include <sol/sol.hpp>
#include "mesh_factory.hpp"

MeshFactory::MeshFactory() = default;
MeshFactory::~MeshFactory() = default;

std::shared_ptr<Mesh> MeshFactory::get_mesh(const std::string& mesh_path) {
    auto it = m_mesh_cache.find(mesh_path);

    if (it != m_mesh_cache.end())
    {
        return it->second;
    }

    std::cerr << "[Mesh Factory] Mesh not preloaded: " << mesh_path << "\n";

    // To-Do: Fallback in here
    
    return nullptr;
}

void MeshFactory::load_mesh(const std::string& mesh_path) {
    sol::state lua;
    lua.open_libraries(sol::lib::base);

    // Try to load Lua file
    sol::table load_result;

    try {
        load_result = lua.script_file(mesh_path);
    } catch (const sol::error& e) {
        std::cerr << "[MeshFactory] Lua error while loading " << mesh_path
                    << ": " << e.what() << "\n";
        
        // To-Do: Fallback in here

        return;
    }

    // Check the value type
    sol::table mesh_lua;

    if (load_result.is<sol::table>())
    {
        mesh_lua = load_result.as<sol::table>();
    }
    else
    {
        std::cerr << "[MeshFactory] " << mesh_path << " did not return a table" << "\n";

        // To-Do: Fallback in here

        return;
    }

    // Attributes
    std::vector<VertexAttribute> attributes;
    sol::table attr_table = mesh_lua["attributes"];

    for (auto&& kv : attr_table)
    {
        sol::table attr = kv.second.as<sol::table>();

        std::string name = attr.get_or("name", std::string{});
        size_t size = attr.get_or("size", 0);

        attributes.push_back({
            name,
            size
        });
    }

    // Vertices
    std::vector<GLfloat> vertex_data;
    
    if (mesh_lua["vertices"].valid())
    {
        vertex_data = mesh_lua["vertices"].get<std::vector<GLfloat>>();
    }
    else
    {
        std::cerr << "[MeshFactory] Missing 'vertices' field in: " << mesh_path << "\n";

        // To-Do: Fallback in here
    }

    // Indices
    std::vector<GLuint> index_data;

    if (mesh_lua["indices"].valid())
    {
        index_data = mesh_lua["indices"].get<std::vector<GLuint>>();
    }
    else
    {
        std::cerr << "[MeshFactory] Missing 'indices' field in: " << mesh_path << "\n";

        // To-Do: Fallback in here
    }

    auto mesh_ptr = std::make_shared<Mesh>(
        vertex_data,
        index_data,
        attributes
    );

    m_mesh_cache[mesh_path] = mesh_ptr;
}

void MeshFactory::clear_cache() {
    m_mesh_cache.clear();
}