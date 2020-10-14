#pragma once

#include <functional>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include "torus.h"

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

    glm::mat4 get_VP(glm::mat4 const & model, glm::vec2 const & position, glm::vec2 const & direction, float ratio
    ) {
        glm::vec3 look_at = glm::vec3(model * glm::vec4(0, 0, 0, 1));
        glm::vec3 up = glm::vec3(model * glm::vec4(0, -1, 0, 0));
        glm::vec3 new_eye = glm::vec3(get_landscape_transform_(position) *
                                       glm::vec4(direction.y / 10, 0.08, -direction.x / 10, 1));

        eye_speed_ += std::max(0.f, glm::distance(current_eye_, new_eye) / 12.f - 0.05f);
        current_eye_ += (new_eye - current_eye_) * eye_speed_;

        auto view = glm::lookAt<float>(current_eye_, look_at, up);
        auto projection = glm::perspective<float>(45, ratio, 0.001f, 25.0f);
        return projection * view;
    }

};