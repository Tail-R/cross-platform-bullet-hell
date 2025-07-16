#include <iostream>
#include "mesh_factory.hpp"

MeshFactory::MeshFactory() = default;
MeshFactory::~MeshFactory() = default;

namespace {
    std::optional<sol::table> load_lua_mesh(sol::state& lua, const std::string& path) {
        try
        {
            sol::object result = lua.script_file(path);

            if (result.is<sol::table>())
            {
                return result.as<sol::table>();
            }
            else
            {
                std::cerr << "[MeshFactory] ERROR: " << path << " did not return a table\n";
            }
        }
        catch (const sol::error& e)
        {
            std::cerr << "[MeshFactory] ERROR: Lua error while loading " << path << ": " << e.what() << "\n";
        }

        return std::nullopt;
    }
}

std::shared_ptr<Mesh> MeshFactory::get_mesh(const std::string& mesh_path) {
    auto it = m_mesh_cache.find(mesh_path);

    if (it != m_mesh_cache.end())
    {
        return it->second;
    }

    std::cerr << "[Mesh Factory] WARNING: Mesh not preloaded: " << mesh_path << "\n";

    // To-Do: Fallback in here
    
    return nullptr;
}

void MeshFactory::load_mesh(sol::state& lua, const std::string& mesh_path) {
    auto mesh_tbl_opt = load_lua_mesh(lua, mesh_path);

    if (!mesh_tbl_opt.has_value())
    {
        return;
    }

    const sol::table& mesh_lua = mesh_tbl_opt.value();

    // Attributes
    std::vector<VertexAttribute> attributes;

    if (mesh_lua["attributes"].valid())
    {
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
    }
    else
    {
        std::cerr << "[MeshFactory] WARNING: Missing 'attributes' field in: " << mesh_path << "\n";

        // To-Do: Fallback in here
        return;
    }

    // Vertices
    std::vector<GLfloat> vertex_data;
    
    if (mesh_lua["vertices"].valid())
    {
        vertex_data = mesh_lua["vertices"].get<std::vector<GLfloat>>();
    }
    else
    {
        std::cerr << "[MeshFactory] WARNING: Missing 'vertices' field in: " << mesh_path << "\n";

        // To-Do: Fallback in here
        return;
    }

    // Indices
    std::vector<GLuint> index_data;

    if (mesh_lua["indices"].valid())
    {
        index_data = mesh_lua["indices"].get<std::vector<GLuint>>();
    }
    else
    {
        std::cerr << "[MeshFactory] WARNING: Missing 'indices' field in: " << mesh_path << "\n";

        // To-Do: Fallback in here
        return;
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