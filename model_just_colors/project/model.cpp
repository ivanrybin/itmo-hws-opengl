#define TINYOBJLOADER_IMPLEMENTATION
#include "model.h"
#include <iostream>

#include <glm/vec3.hpp>
#include <set>


namespace my_model {

    glm::vec2 get_vec2_tinyobj(const std::vector<tinyobj::real_t>& v, size_t start_pos) {
        return glm::vec2(v[start_pos + 0],
                         v[start_pos + 1]);
    }

    glm::vec3 get_vec3_tinyobj(const std::vector<tinyobj::real_t>& v, size_t start_pos) {
        return glm::vec3(v[start_pos + 0],
                         v[start_pos + 1],
                         v[start_pos + 2]);
    }

    model::model(const std::string& file, const std::string& dir_model) : dir_path(dir_model) {
        std::string error;

        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &error, file.c_str(), dir_model.c_str());

        if (!error.empty()) {
            std::cerr << "TINYOBJ ERROR -- model: " << file << std::endl;
            std::cerr << error << std::endl;
        }
        if (!ret) {
            std::cerr << "TINYOBJ NOT LOADED -- model: " << file << std::endl;
        }

        init();
    }

    void model::load_textures() {
        for (const auto& mat : materials) {
            if (!mat.diffuse_texname.empty()) {
                if (texture_indices.count(dir_path + "/" + mat.diffuse_texname) == 0) {
                    texture t(dir_path + "/" + mat.diffuse_texname);
                    t.type = "diffuse";
                    t.init();
                    texture_indices[dir_path + "/" + mat.diffuse_texname] = textures.size();
                    textures.push_back(t);
                    std::cout << "diffuse loaded" << std::endl;
                }
            }
            if (!mat.specular_texname.empty()) {
                if (texture_indices.count(dir_path + "/" + mat.specular_texname) == 0) {
                    texture t(dir_path + "/" + mat.specular_texname);
                    t.type = "specular";
                    t.init();
                    texture_indices[dir_path + "/" + mat.specular_texname] = textures.size();
                    textures.push_back(t);
                    std::cout << "specular loaded" << std::endl;
                }
            }
        }
    }


    void model::init() {
        // preload all textures
        load_textures();

        for (const auto& shape : shapes) {

            uint32_t v_idx = 0;
            std::vector<vertex>     mesh_vertices{};
            std::vector<uint32_t>   mesh_indices{};

            for (const auto& index : shape.mesh.indices) {
                glm::vec3 position   = get_vec3_tinyobj(attrib.vertices,  3 * index.vertex_index);
                glm::vec3 normal     = get_vec3_tinyobj(attrib.normals,   3 * index.normal_index);
                glm::vec2 tex_coords = get_vec2_tinyobj(attrib.texcoords, 2 * index.texcoord_index);

                // Optional: vertex colors
                // glm::vec3 colors = get_vec3_tinyobj(attrib.colors, 3 * index.vertex_index);

                vertex v = {position, normal, tex_coords};
                mesh_vertices.push_back(v);
                mesh_indices.push_back(v_idx);

                ++v_idx;
            }

            // textures for mesh
            std::set<int> mat_ids(shape.mesh.material_ids.begin(), shape.mesh.material_ids.end());
            std::vector<texture> mesh_textures{};

            for (const auto& mat_id : mat_ids) {
                if (!materials[mat_id].diffuse_texname.empty()) {
                    mesh_textures.push_back(textures[texture_indices[dir_path + "/" + materials[mat_id].diffuse_texname]]);
                }
                if (!materials[mat_id].specular_texname.empty()) {
                    mesh_textures.push_back(textures[texture_indices[dir_path + "/" + materials[mat_id].specular_texname]]);
                }
            }

            meshes.emplace_back(mesh_indices, mesh_vertices, mesh_textures);
        }
    }
}
