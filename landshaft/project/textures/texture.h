#pragma once
#include <string>
#include <utility>
#include <vector>

namespace my_model {

    struct texture {
        texture(std::string path) : path(std::move(path)) {}

        void init();

        unsigned int id;

        std::string type;
        std::string path;

        unsigned char* data = nullptr;

        int width, height, nrChannels;
    };

    unsigned int cube_map_loader(const std::vector<std::string>& pathes);
}