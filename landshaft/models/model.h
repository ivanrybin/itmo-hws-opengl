#pragma once

#include "../../bindings/tiny_obj_loader.h"

#include "mesh.h"

namespace my_model {

    struct model {
        model(const std::string& file, const std::string& dir_model);

        void init();

        void draw(shader_t& shader) const {
            for (const auto& m : meshes) {
                m.draw(shader);
            }
        }

        void load_textures();

        tinyobj::attrib_t                 attrib;
        std::vector<tinyobj::shape_t>     shapes;
        std::vector<tinyobj::material_t>  materials;

        std::vector<mesh>    meshes;
        std::vector<texture> textures;

        std::map<std::string, uint32_t> texture_indices;

        std::string dir_path;

    };
}