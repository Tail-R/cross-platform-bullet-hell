#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include "../mesh/mesh.hpp"

class MeshFactory {
public:
    MeshFactory();
    ~MeshFactory();

    std::shared_ptr<Mesh> get_mesh(const std::string& mesh_path);
    void load_mesh(const std::string& mesh_path);

    void clear_cache();

private:
    std::unordered_map<std::string, std::shared_ptr<Mesh>> m_mesh_cache;
};