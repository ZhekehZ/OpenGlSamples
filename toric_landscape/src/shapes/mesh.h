#pragma once

#include <GL/glew.h>
#include <vector>
#include <iostream>
#include <utility>
#include <filesystem>
#include <glm/glm.hpp>
#include <map>
#include <glm/gtc/type_ptr.hpp>

#include <tiny_obj_loader.h>

#include <src/shaders/opengl_shader.h>
#include <src/shadows/light_system.h>
#include <src/io/texture_loader.h>

namespace detail {
    auto FLOAT_MIN = std::numeric_limits<float>::min();
    auto FLOAT_MAX = std::numeric_limits<float>::max();

    template <typename ... Floats>
    float max_float(float f, Floats ... floats) {
        if constexpr (sizeof...(Floats) == 0) return f;
        else return std::max(f, max_float(floats...));
    }

    glm::vec3 min(glm::vec3 const &v1, glm::vec3 const &v2) {
        return {std::min(v1.x, v2.x), std::min(v1.y, v2.y), std::min(v1.z, v2.z)};
    }

    glm::vec3 max(glm::vec3 const &v1, glm::vec3 const &v2) {
        return {std::max(v1.x, v2.x), std::max(v1.y, v2.y), std::max(v1.z, v2.z)};
    }
}


template <int TextureSlot>
class Mesh {
    struct BBox {
        glm::vec3 low = {detail::FLOAT_MAX, detail::FLOAT_MAX, detail::FLOAT_MAX};
        glm::vec3 high = {detail::FLOAT_MIN, detail::FLOAT_MIN, detail::FLOAT_MIN};
    };

    GLuint vao_ = 0;
    std::vector<std::tuple<GLuint, int, int>> parts_;
    BBox bounds_;
    Shader shader_;

public:
    explicit Mesh(std::filesystem::path const & path, const Shader& shader)
        : shader_(shader)
    {
        load_OBJ(path);
    }

    float get_size() const noexcept {
        auto sizes = bounds_.high - bounds_.low;
        return detail::max_float(sizes.x, sizes.y, sizes.z, 0.0f);
    }

    template <typename Shadow>
    void draw(glm::mat4 model, glm::mat4 view, glm::mat4 projection, LightSystem<Shadow> & lights) {
        glm::mat4 mvp = projection * view * model;
        glm::mat4 vmn = glm::transpose(glm::inverse(model));

        shader_.use();
        shader_.set_uniform("u_mvp", glm::value_ptr(mvp));
        shader_.set_uniform("u_model", glm::value_ptr(model));
        shader_.set_uniform("u_mv_normal", glm::value_ptr(vmn));
        shader_.set_uniform("u_texture", TextureSlot);
        auto near_mvp = lights[light_t::GLOBAL_NEAR].get_MVP(model);
        shader_.set_uniform("u_mvp_light_near", glm::value_ptr(near_mvp));
        shader_.set_uniform("u_shadow", lights[light_t::GLOBAL_NEAR].get_current_slot());

        glBindVertexArray(vao_);
        for (auto [texture, first, size] : parts_) {
            glActiveTexture(GL_TEXTURE0 + TextureSlot);
            glBindTexture(GL_TEXTURE_2D, texture);
            glDrawArrays(GL_TRIANGLES, first, size);
        }
    }

    void draw() {
        glBindVertexArray(vao_);
        for (auto [texture, first, size] : parts_) {
            glDrawArrays(GL_TRIANGLES, first, size);
        }
    }

    float get_height() const {
        return (bounds_.high - bounds_.low).y;
    }

    float get_width() const {
        return (bounds_.high - bounds_.low).x;
    }

    float get_length() const {
        return (bounds_.high - bounds_.low).z;
    }

private:
    void load_OBJ(std::filesystem::path const & path) {
        using namespace tinyobj;
        using namespace detail;

        // Read data from .obj && .mtl
        attrib_t attrib;
        std::vector<shape_t> shapes;
        std::vector<material_t> materials;

        std::string err;

        bool res = LoadObj(&attrib, &shapes, &materials, &err, path.string().c_str(),
                           (path.parent_path().string() + "/").c_str());

        if (!res) {
            std::cerr << "Load_OBJ error: " << err << std::endl;
        } else if (!err.empty()) {
            std::cerr << "Load_OBJ warning: " << err << std::endl;
        }


        std::map<std::string, GLuint> textures;
        auto get_texture = [&](shape_t const & shape) -> std::pair<bool, GLuint> {
            std::string tex_name;
            for (auto i : shape.mesh.material_ids) {
                const auto & diffuse = materials[i].diffuse_texname;
                if (tex_name.empty()) {
                    tex_name = diffuse;
                } else if (tex_name != diffuse && !diffuse.empty()) {
                    throw std::runtime_error("Only one texture per shape allowed");
                }
            }
            if (tex_name.empty()) return {false, 0};
            auto it = textures.find(tex_name);
            if (it != textures.end()) return {true, it->second};
            textures[tex_name] = load_single_texture(path.parent_path().append(tex_name).string());
            return {true, textures[tex_name]};
        };

        auto get_point = [&](index_t const & index) -> glm::vec3 {
            auto result =  glm::vec3(attrib.vertices[index.vertex_index * 3 + 0],
                             attrib.vertices[index.vertex_index * 3 + 1],
                             attrib.vertices[index.vertex_index * 3 + 2]);
            bounds_.low = min(bounds_.low, result);
            bounds_.high = max(bounds_.high, result);
            return result;
        };

        auto get_normal = [&](index_t const & index) -> glm::vec3 {
            return glm::vec3(attrib.normals[index.normal_index * 3 + 0],
                             attrib.normals[index.normal_index * 3 + 1],
                             attrib.normals[index.normal_index * 3 + 2]);
        };

        auto get_color = [&](shape_t const & shape, int face_index) -> glm::vec3 {
            if (shape.mesh.material_ids.empty()) return { 1, 1, 1 };
            auto & mat = materials[shape.mesh.material_ids[face_index]].diffuse;
            return {mat[0], mat[1], mat[2]};
        };

        auto get_emission_color = [&](shape_t const& shape, int face_index) -> glm::vec3 {
            if (shape.mesh.material_ids.empty()) return { 0, 0, 0 };
            auto& mat = materials[shape.mesh.material_ids[face_index]].emission;
            return { mat[0], mat[1], mat[2] };
        };

        auto get_tex = [&](shape_t const & shape, index_t const & index, int face_index, bool is_textured) -> glm::vec3 {
            if (!is_textured || attrib.texcoords.empty()) return {0.0f, 0.0f, 0.0f};
            float mix = materials[shape.mesh.material_ids[face_index]].specular[0];
            return {attrib.texcoords[index.texcoord_index * 2 + 0],
                    attrib.texcoords[index.texcoord_index * 2 + 1],
                    mix < 1e-7 ? 0 : 1};
        };

        // Create VBO

        int constexpr COMPONENTS = 5;

        std::vector<glm::vec3> V;
        for (auto const & shape : shapes) {
            int first = int(V.size()) / COMPONENTS;

            auto [is_textured, texture] = get_texture(shape);

            // Fill buffer
            int face_index = 0;
            for (auto i = 0u; i < shape.mesh.indices.size(); i += shape.mesh.num_face_vertices[face_index++]) {

                const auto & color = get_color(shape, face_index);
                const auto & emission = get_emission_color(shape, face_index);

                if (shape.mesh.num_face_vertices[face_index] != 3) {
                    throw std::runtime_error("The mesh is not triangulated");
                }

                for (unsigned j = 0; j < 3; ++j) {
                    auto idx = shape.mesh.indices[i + j];

                    V.push_back(get_point(idx));
                    V.push_back(get_normal(idx));
                    V.push_back(color);
                    V.push_back(emission);
                    V.push_back(get_tex(shape, idx, face_index, is_textured));
                }
            }

            int size = int(V.size()) / COMPONENTS - first;
            parts_.emplace_back(texture, first, size);
        }

        int stride = COMPONENTS * sizeof(V[0]);

        glGenVertexArrays(1, &vao_);
        glBindVertexArray(vao_);

        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, V.size() * sizeof(V[0]), V.data(), GL_STATIC_DRAW);

        for (int i = 0; i < COMPONENTS; ++i) {
            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, stride, (void *) (i * 3l * sizeof(float)));
        }
        glBindVertexArray(0);
    }
};