#pragma once

#include <functional>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include "src/shapes/torus.h"

class Camera {
    glm::vec3 current_eye_ = {0, 0, 0};
    float eye_speed_ = 0;
    std::function<glm::mat4(glm::vec2 const & pos)> get_landscape_transform_;

public:
    template <int x, int y, int z>
    explicit Camera(Torus<x, y, z> const & tor)
        : get_landscape_transform_(
            [&](glm::vec2 const & pos) {
                return tor.get_transformation_to_pos(pos);
            }
        )
    {};

    static glm::mat4 get_lightVP(
            glm::mat4 const & model,
            glm::vec2 const & position,
            glm::vec2 const & direction
    ) {
        static float d = 0;
        if (ImGui::IsKeyPressed(GLFW_KEY_R)) {
            d += 0.01;
            std::cout << d << '\n';
        }
        if (ImGui::IsKeyPressed(GLFW_KEY_T)) {
            d -= 0.01;
            std::cout << d << '\n';
        }
        glm::vec3 look_at = glm::vec3(model * glm::vec4(0, 0, 1, 1));
        glm::vec3 up = glm::vec3(model * glm::vec4(0, 1, 0, 0));
        glm::vec3 eye = glm::vec3(model * glm::vec4(0, 0.01, 0.03, 1));

        auto view = glm::lookAt(eye, look_at, up);
        auto projection = glm::perspective(glm::radians(30.f), 1.f, 0.001f, 0.2f);
        return projection * view;
    }

    std::pair<glm::mat4, glm::mat4> get_VP(
            glm::mat4 const & model,
            glm::vec2 const & position,
            glm::vec2 const & direction,
            float ratio
    ) {
        glm::vec3 look_at = glm::vec3(model * glm::vec4(0, 0, 0, 1));
        glm::vec3 up = glm::vec3(model * glm::vec4(0, -1, 0, 0));
        glm::vec3 new_eye = glm::vec3(get_landscape_transform_(position) *
                                       glm::vec4(direction.y / 10, 0.08, -direction.x / 10, 1));

        eye_speed_ += std::max(0.f, glm::distance(current_eye_, new_eye) / 12.f - 0.07f);
        auto d = glm::distance(current_eye_, new_eye);
        current_eye_ += (new_eye - current_eye_) * 0.3f;
        if (glm::distance(current_eye_, new_eye) < d) current_eye_ = new_eye;

        auto view = glm::lookAt(current_eye_, look_at, up);
        auto projection = glm::perspective(glm::radians(80.f), ratio, 0.001f, 25.f);
        return {view, projection};
    }

};