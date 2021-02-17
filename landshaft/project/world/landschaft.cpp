#include "landschaft.h"

#include <iostream>

#include "../../bindings/stb_image.h"

namespace my_model {

    landschaft::landschaft(const std::string& hmap_file, const std::string& text_file, int step) : step(step) {
        int width_hmap, height_hmap, numb_channels_hmap;
        unsigned char* hmap_data = stbi_load(hmap_file.c_str(), &width_hmap, &height_hmap, &numb_channels_hmap, 0);
        if (!hmap_data) {
            std::cout << "Failed to load height map: " << hmap_file << std::endl;
            return;
        }

        std::vector<std::vector<int>> hmap_coords(height_hmap, std::vector<int>(width_hmap, 0));
        for (int y = 0; y < height_hmap; y += step) {
            for (int x = 0; x < width_hmap; x += step) {
                hmap_coords[y][x] = hmap_data[y * width_hmap + x];
            }
        }
        stbi_image_free(hmap_data);

        width_hmap = hmap_coords.size();
        height_hmap = hmap_coords[0].size();

        std::vector<std::vector<glm::vec3>> vertices_grid(height_hmap, std::vector<glm::vec3>{});
        for (int z = 0; z < height_hmap; ++z) {
            for (int x = 0; x < width_hmap; ++x) {
                float X = 2.0f * x / width_hmap - 1.0f;
                float Y = hmap_coords[z][x] / 255.0f;
                float Z = 2.0f * z / height_hmap - 1.0f;

                double i;
                double f = modf(Y * 10000, &i);
                Y = Y - f / 10000;

                glm::vec3 position(X, Y, Z);
                vertices_grid[z].push_back(position);
            }
        }

        for (int z = 0; z < height_hmap; ++z) {
            for (int x = 0; x < width_hmap; ++x) {

                glm::vec3 position = vertices_grid[z][x];
                glm::vec3 normal = get_normal(z, x, vertices_grid, width_hmap, height_hmap);
                glm::vec2 texture_coords(position.x, position.z);

                vertices.push_back({position, normal, texture_coords});
            }
        }

        for (int z = 0; z + step < height_hmap; ++z) {
            for (int x = 0; x + step < width_hmap; ++x) {
                indices.push_back(x + z * width_hmap);
                indices.push_back((x + 1) + z * width_hmap);
                indices.push_back(x + (z + 1) * width_hmap);

                indices.push_back((x + 1) + z * width_hmap);
                indices.push_back(x + (z + 1) * width_hmap);
                indices.push_back((x + 1) + (z + 1) * width_hmap);
            }
        }

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Координаты вершин
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *) 0);

        // Координаты вершин
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *) sizeof(glm::vec3));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *) (2 * sizeof(glm::vec3)));

        glBindVertexArray(0);
    }

    void landschaft::draw(shader_t &shader, unsigned int texture_id) const {
        glActiveTexture(GL_TEXTURE0 + texture_id);
        shader.set_uniform<unsigned int>("worldTexture", texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    glm::vec3 landschaft::get_normal(const glm::vec3& v1, const glm::vec3& v2) {
        return cross(v2, v1);
    }

    glm::vec3 landschaft::normalize_normal(
            glm::vec3& n1,
            glm::vec3& n2,
            glm::vec3& n3,
            glm::vec3& n4
    ) {
        return normalize(n1 + n2 + n3 + n4);
    }

    glm::vec3 landschaft::get_normal(
            size_t z,
            size_t x,
            const std::vector<std::vector<glm::vec3>>& grid,
            size_t width_hmap,
            size_t height_hmap
    ) {
        int z1 = z == 0 ? height_hmap - 2 : z - 1;
        int z2 = z == height_hmap - 1 ? 1 : z + 1;
        int x1 = x == 0 ? width_hmap - 2 : x - 1;
        int x2 = x == width_hmap - 1 ? 1 : x + 1;

        glm::vec3 a = grid[z2][x];
        glm::vec3 b = grid[z][x1];
        glm::vec3 c = grid[z1][x];
        glm::vec3 d = grid[z][x2];
        glm::vec3 v = grid[z][x];

        auto n1 = get_normal(
                { a[0] - v[0], a[1] - v[1], a[2] - v[2] },
                { b[0] - v[0], b[1] - v[1], b[2] - v[2] }
        );
        auto n2 = get_normal(
                { b[0] - v[0], b[1] - v[1], b[2] - v[2] },
                { c[0] - v[0], c[1] - v[1], c[2] - v[2] }
        );
        auto n3 = get_normal(
                { c[0] - v[0], c[1] - v[1], c[2] - v[2] },
                { d[0] - v[0], d[1] - v[1], d[2] - v[2] }
        );
        auto n4 = get_normal(
                { d[0] - v[0], d[1] - v[1], d[2] - v[2] },
                { a[0] - v[0], a[1] - v[1], a[2] - v[2] }
        );

        return normalize_normal(n1, n2, n3, n4);
    }
}
