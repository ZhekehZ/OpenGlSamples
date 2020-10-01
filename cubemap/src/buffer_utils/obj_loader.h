#pragma once

#include <vector>
#include <utility>
#include <filesystem>
#include "tiny_obj_loader.h"

using VBuffer = std::vector<float>;
using IBuffer = std::vector<int>;

std::pair<VBuffer, IBuffer> load_OBJ(std::filesystem::path const & path) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;
    std::string warn;

    bool res = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.string().c_str(),
                                (path.parent_path().string() + "/").c_str());

    if (!res) {
        std::cerr << "LoadOBJ error: " << err << std::endl;
    }

    std::vector<float> V;
    std::vector<int> F;

    for (auto const & shape : shapes) {
        for (auto i = 0u; i < shape.mesh.indices.size(); ++i) {
            auto idx = shape.mesh.indices[i];

            F.push_back(int(F.size()));
            for (auto j = 0u; j < 3; ++j)
                V.push_back(attrib.vertices[idx.vertex_index * 3 + j]);
            for (auto j = 0u; j < 3; ++j)
                V.push_back(attrib.normals[idx.normal_index * 3 + j]);

            if (!materials.empty()) {
                const auto &mat = materials[shape.mesh.material_ids[i / 3]];
                for (float j : mat.diffuse) V.push_back(j);
                V.push_back(mat.dissolve);
//                throw std::runtime_error("empty");
            } else {
                for (auto j = 0u; j < 4; ++j) V.push_back(1);
            }
        }
    }

    return {V, F};
}