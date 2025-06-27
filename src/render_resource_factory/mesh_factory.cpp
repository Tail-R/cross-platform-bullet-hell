#include "mesh_factory.hpp"

MeshFactory::MeshFactory() {

}

MeshFactory::~MeshFactory() {

}

std::shared_ptr<Mesh> MeshFactory::get_mesh(const std::string& mesh_path) {

}

void MeshFactory::load_mesh(const std::string& mesh_path) {

}

void MeshFactory::clear_cache() {
    m_shader_cache.clear();
}