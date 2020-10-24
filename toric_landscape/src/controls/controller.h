#pragma once

#include <glm/glm.hpp>
#include <imgui.h>

class Controller {
    float velocity_ = 0.0f;
    float dir_angle_ = 0.0f;
    glm::vec2 position_{0, 0.5};
    glm::vec2 direction_{0, 1};
    float direction_ratio_ = 1.0f;

    std::function<glm::mat4(glm::vec2)> get_global_rotation_{};

public:
    template <int x, int y, int z>
    explicit Controller(Torus<x, y, z> const & tor)
        : get_global_rotation_(
            [&](glm::vec2 const & pos) {
                return tor.get_rotation(pos);
            }
        )
        , direction_ratio_(tor.get_ratio())
    {};

    void handle_keys() {
        if (ImGui::IsKeyDown(GLFW_KEY_W)) {
            velocity_ += 0.0001f;
        }
        if (ImGui::IsKeyDown(GLFW_KEY_S)) {
            velocity_ -= 0.0001f;
        }

        if (ImGui::IsKeyDown(GLFW_KEY_D)) {
            dir_angle_ += 30 * velocity_;
        }
        if (ImGui::IsKeyDown(GLFW_KEY_A)) {
            dir_angle_ -= 30 * velocity_;
        }

        velocity_ /= 1.2f;
        velocity_ = std::max(std::min(velocity_, .3f), -.3f);
        if (abs(velocity_) < 0.000001) velocity_ = 0;

        direction_ = {cos(dir_angle_), sin(dir_angle_)};
    }

    void update() {
        position_ += direction_ * (velocity_ * glm::vec2(direction_ratio_, 1.f)) / 2.f;
    }

    glm::mat4 get_rotation() const {
        return glm::rotate(-dir_angle_, glm::vec3(0, 1, 0));
    }

    glm::mat4 get_full_rotation() const {
        return get_global_rotation_(position_) * glm::rotate(-dir_angle_, glm::vec3(0, 1, 0));
    }


    glm::mat4 rotate_to_direction(glm::mat4 const & mat) const {
        return get_rotation() * mat;
    }

    const glm::vec2 & get_position() const {
        return position_;
    }

    glm::vec2 get_direction() const {
        return glm::normalize(direction_);
    }
};