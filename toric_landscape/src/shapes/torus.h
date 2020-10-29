#pragma once
#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include <glm/gtx/rotate_vector.hpp>

#include <cmath>
#include <src/shadows/light_system.h>

#define M_PI   3.14159265358979323846   // pi
#define M_PI_2 1.57079632679489661923   // pi/2


template <int TextureSlot1, int TextureSlot2, int TextureSlot3, int DetailTextureSlot>
class Torus {
    GLuint vao_{};
    std::size_t size_ = 0;

    unsigned char * image_;
    int width_{}, height_{}, channels_{};

    int steps1_ = 1000;
    int steps2_ = 100;

    int w_factor_ = 6;
    int h_factor_ = 1;

    float max_height_ = 0.4f;

    float R_, r_;

    Shader shader_;
    GLuint t1_, t2_, t3_, detail_;

     float get_height(int i, int j) const {
        int img_x = (i * width_ * w_factor_ / steps1_) % width_;
        int img_y = (j * height_ * h_factor_ / steps2_) % height_;
        img_x = (img_x + width_) % width_;
        img_y = (img_y + height_) % height_;
        return float(image_[3 * (img_x * height_ + img_y)]) / 255.0f;
    };

    glm::vec3 get_point(int i, int j, int shift_x = 0, int shift_y = 0) const {
        auto phi = static_cast<float>((M_PI * 2.0 * (i + shift_x)) / steps1_);
        auto psi = static_cast<float>(M_PI * (2.0 * (j + shift_y) / steps2_ - 1));
        float h = get_height(i, j) * max_height_;

        return {
            (R_ + (r_ + h) * cos(psi)) * cos(phi),
            (R_ + (r_ + h) * cos(psi)) * sin(phi),
            (r_ + h) * sin(psi)
        };
    };

     glm::vec3 get_tex_pos(int i, int j) const {
        return {float(i * w_factor_ * 40) / float(steps1_),
                float(j * h_factor_ * 40) / float(steps2_),
                get_height(i, j)};
    };

     glm::vec3 get_normal(int i, int j, int shift_x = 0, int shift_y = 0) const {
        glm::vec3 p = get_point(i, j, shift_x, shift_y);
        glm::vec3 vs[] = {
                get_point(i + 1, j, shift_x, shift_y) - p,
                get_point(i, j - 1, shift_x, shift_y) - p,
                get_point(i - 1, j, shift_x, shift_y) - p,
                get_point(i, j + 1, shift_x, shift_y) - p
        };
        auto ncp = [](auto & a, auto & b) { return normalize(cross(a, b)); };
        return (ncp(vs[1], vs[0]) + ncp(vs[2], vs[1]) +
                ncp(vs[3], vs[2]) + ncp(vs[0], vs[3])) / 4.0f;
    };

     glm::vec3 get_relative_normal(int i, int j) const {
        return get_normal(i, j, -i + steps1_ / 4, -j + steps2_ / 2);
    };

public:
    Torus(  
        std::string const & height_map, 
        float R, 
        float r, 
        float max_height,
        Shader const & shader, 
        path texture_path,
        std::string const & p1, 
        std::string const & p2, 
        std::string const & p3,
        std::string const & detail_texture
    ) : R_(R)
      , r_(r)
      , max_height_(max_height)
      , shader_(shader)
    {
        using namespace glm;

        texture_path.append(p1);
        t1_ = load_single_texture(texture_path.string());
        texture_path.replace_filename(p2);
        t2_ = load_single_texture(texture_path.string());
        texture_path.replace_filename(p3);
        t3_ = load_single_texture(texture_path.string());
        texture_path.replace_filename(detail_texture);
        detail_ = load_single_texture(texture_path.string());

        glActiveTexture(GL_TEXTURE0 + TextureSlot1);
        glBindTexture(GL_TEXTURE_2D, t1_);
        glActiveTexture(GL_TEXTURE0 + TextureSlot2);
        glBindTexture(GL_TEXTURE_2D, t2_);
        glActiveTexture(GL_TEXTURE0 + TextureSlot3);
        glBindTexture(GL_TEXTURE_2D, t3_);
        glActiveTexture(GL_TEXTURE0 + DetailTextureSlot);
        glBindTexture(GL_TEXTURE_2D, detail_);

        image_ = stbi_load(height_map.c_str(), &width_, &height_, &channels_, STBI_rgb);

        if (!image_) std::cerr << "ERROR LOADING IMAGE " << height_map << std::endl;

        std::vector<vec3> V;
        std::vector<int> I;

        int limX = steps1_ + 1;
        int limY = steps2_ + 1;
        for (int i = 0; i < limX; ++i) {
            for (int j = 0; j < limY; ++j) {
                vec3 v = get_point(i, j);
                vec3 n = get_normal(i, j);
                vec3 t = get_tex_pos(i, j);

                V.push_back(v);
                V.push_back(n);
                V.push_back(t);

                if (i < limX - 1 && j < limY - 1) {
                    int a = i * limY + j;
                    int b = i * limY + j + 1;
                    int c = (i + 1) * limY + j;
                    int d = (i + 1) * limY + j + 1;

                    I.push_back(a), I.push_back(b), I.push_back(d);
                    I.push_back(d), I.push_back(c), I.push_back(a);
                }
            }
        }

        glGenVertexArrays(1, &vao_);
        glBindVertexArray(vao_);

        size_ = I.size();

        GLuint vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, V.size() * sizeof(V[0]), V.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_ * sizeof(int), I.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), nullptr);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(6 * sizeof(float)));

        glBindVertexArray(0);
    }

     float get_ratio() const {
        return float(width_ * w_factor_) / float(height_ * h_factor_);
    }

    template <typename Shadow>
    void draw(glm::mat4 mvp, LightSystem<Shadow> & lights, float time) {
        shader_.use();
        shader_.set_uniform("u_time", time);
        shader_.set_uniform("texture1", TextureSlot1);
        shader_.set_uniform("texture2", TextureSlot2);
        shader_.set_uniform("texture3", TextureSlot3);
        shader_.set_uniform("u_detail", DetailTextureSlot);
        shader_.set_uniform("u_mvp", glm::value_ptr(mvp));

        auto near_mvp = lights[light_t::GLOBAL_NEAR].get_VP();
        auto far_mvp = lights[light_t::GLOBAL_FAR].get_VP();
        auto dir_mvp = lights[light_t::DIRECTIONAL1].get_VP();
        
        shader_.set_uniform("u_mvp_light_near", glm::value_ptr(near_mvp));
        shader_.set_uniform("u_mvp_light_far", glm::value_ptr(far_mvp));
        shader_.set_uniform("u_mvp_light_dir", glm::value_ptr(dir_mvp));
        shader_.set_uniform("u_shadow_near", lights[light_t::GLOBAL_NEAR].get_current_slot());
        shader_.set_uniform("u_shadow_far", lights[light_t::GLOBAL_FAR].get_current_slot());

        glBindVertexArray(vao_);
        glDrawElements(GL_TRIANGLES, GLsizei(size_), GL_UNSIGNED_INT, nullptr);
    }

    void draw() {
        glBindVertexArray(vao_);
        glDrawElements(GL_TRIANGLES, GLsizei(size_), GL_UNSIGNED_INT, nullptr);
    }

     float get_height_from_pos(float x, float y) const {
        std::swap(x, y);
        auto const & [coords, a, b, low] = get_pseudo_barycentric_coords(x, y);
        glm::vec3 hs = glm::vec3{low ? get_height(a, b) : get_height(a + 1, b + 1),
                                       get_height(a + 1, b), get_height(a, b + 1)};
        return r_ + glm::dot(coords, hs) * max_height_;
    }

     glm::mat4 get_rotation(glm::vec2 const & pos) const {
        auto const & [coords, a, b, low] = get_pseudo_barycentric_coords(pos.y, pos.x);
        glm::vec3 normal =
                (low ? coords.x * get_relative_normal(a, b) : coords.x * get_relative_normal(a + 1, b + 1)) +
                coords.y * get_relative_normal(a + 1, b) +
                coords.z * get_relative_normal(a, b + 1);
        return glm::orientation(normal, glm::vec3(0, 1, 0));
    }

     static std::pair<float, float> get_angles_from_pos(glm::vec2 const & pos) {
        auto phi = static_cast<float>(M_PI * 2.0 * pos.x);
        auto psi = static_cast<float>(M_PI * (2.0 * pos.y - 1));
        return {phi + float(M_PI), psi + float(M_PI_2)};
    }

    template <bool fixed_height = false>
     glm::mat4 get_transformation_to_pos(glm::vec2 const & pos, float bottom_height) const {
        auto [alpha, beta] = get_angles_from_pos(pos);
        glm::mat4 transformation = glm::rotate(beta, glm::vec3(0, 0, 1)) *
                                   glm::translate(glm::vec3(0, R_, 0)) *
                                   glm::rotate(alpha, glm::vec3(1, 0, 0));
        if constexpr (fixed_height) return transformation;
        return transformation *
                glm::translate(glm::vec3(0, get_height_from_pos(pos.x, pos.y) + bottom_height, 0));
    }

private:
     std::tuple<glm::vec3, int, int, bool> get_pseudo_barycentric_coords(float x, float y) const {
        auto w = float(steps1_);
        auto h = float(steps2_);
        x -= floor(x);
        y -= floor(y);

        float a = x * w - floor(x * w);
        float b = y * h - floor(y * h);

        bool low = a + b < 1;
        auto coords = low ? glm::vec3{1 - a - b, a, b}
                          : glm::vec3{a + b - 1, 1 - b, 1 - a};
        return {coords, int(x * w), int(y * h), low};
    }
};
