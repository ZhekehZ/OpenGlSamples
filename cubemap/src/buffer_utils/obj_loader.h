#pragma once

#include <iostream>
#include <vector>
#include <utility>
#include <filesystem>
#include "tiny_obj_loader.h"
#include "mesh.h"

Mesh load_OBJ(std::filesystem::path const & path) {
    // Read data from .obj && .mtl
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;

    bool res = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.string().c_str(),
                                (path.parent_path().string() + "/").c_str());

    if (!res) {
        std::cerr << "Load_OBJ error: " << err << std::endl;
    } else if (!err.empty()) {
        std::cerr << "Load_OBJ warning: " << err << std::endl;
    }

    // Create VBO's
    std::map<std::string, GLuint> textures;
    Mesh mesh;

    for (auto const & shape : shapes) {
        MeshPart mesh_part;
        std::vector<float> V;
        std::vector<int> F;

        // Get texture
        std::string tex_name;
        for (auto i : shape.mesh.material_ids) {
            if (!(tex_name = materials[i].diffuse_texname).empty()) break; // single texture per shape
        }
        if (!tex_name.empty()) {
            if (textures.find(tex_name) == textures.end()) {
                auto file = path.parent_path().append(tex_name);
                textures[tex_name] = load_single_texture(file.string());
            }
            mesh_part.texture = textures[tex_name];
            mesh_part.with_texture = true;
        } else {
            mesh_part.with_texture = false;
            std::string s;
            for (int i : shape.mesh.material_ids) {
                s = materials[i].diffuse_texname;
                if (!s.empty()) break;
            }
        }

        // Fill buffer
        for (auto i = 0u; i < shape.mesh.indices.size(); ++i) {
            auto idx = shape.mesh.indices[i];

            F.push_back(int(F.size()));

            for (auto j = 0u; j < 3; ++j)
                V.push_back(attrib.vertices[idx.vertex_index * 3 + j]); // position: x, y, z

            if (!attrib.normals.empty()) {
                for (auto j = 0u; j < 3; ++j)
                    V.push_back(attrib.normals[idx.normal_index * 3 + j]);  //   normal: x, y, z
            } else {
                V.push_back(0);
                V.push_back(1);
                V.push_back(0);
            }

            if (!shape.mesh.material_ids.empty()) {                     //    color: r, g, b, a
                const auto &mat = materials[shape.mesh.material_ids[i / 3]];
                for (float j : mat.diffuse) V.push_back(j);   // rgb
                V.push_back(mat.dissolve);                    // alpha
            } else {
                for (auto j = 0u; j < 4; ++j) V.push_back(1); // white
            }

            if (!attrib.texcoords.empty()) {
                V.push_back(attrib.texcoords[idx.texcoord_index * 2]);  //  texture: u, v
                V.push_back(attrib.texcoords[idx.texcoord_index * 2 + 1]);
            } else {
                V.push_back(0); V.push_back(0);
            }
        }

        int stride = (3 + 3 + 4 + 2) * sizeof(float);

        glGenVertexArrays(1, &mesh_part.vao);
        glBindVertexArray(mesh_part.vao);

        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, V.size() * sizeof(float), V.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride, (void *)(6 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void *)(10 * sizeof(float)));

        glBindVertexArray(0);

        mesh_part.size = int(V.size()) / (3 + 3 + 4 + 2);

        mesh.parts.push_back(mesh_part);
    }

    return mesh;
}