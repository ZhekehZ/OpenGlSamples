#pragma once

#include <functional>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include "src/shapes/torus.h"

class Camera {
    glm::vec3 current_eye_ = {0, 0, 0};
    std::function<glm::mat4(glm::vec2 const & pos, float)> get_landscape_transform_;

public:
    template <int x, int y, int z>
    explicit Camera(Torus<x, y, z> const & tor)
        : get_landscape_transform_(
            [&](glm::vec2 const & pos, float delta) {
                return tor.get_transformation_to_pos(pos, delta);
            }
        )
    {};

    std::pair<glm::mat4, glm::mat4> get_VP(
            glm::mat4 const & model_mat,
            float height_delta,
            Controller const & controller,
            float ratio
    ) {
        glm::vec2 position = controller.get_position();
        glm::vec2 direction = controller.get_direction();
        auto model = get_landscape_transform_(position, height_delta) * controller.rotate_to_direction(model_mat);

        glm::vec3 look_at = glm::vec3(get_landscape_transform_(position, 0) * glm::vec4(0, 0, 0, 1));
        glm::vec3 up = glm::vec3(model * glm::vec4(0, 0, 1, 0));
        glm::vec3 new_eye = glm::vec3(get_landscape_transform_(position, 0) *
                                       glm::vec4(direction.y / 10, 0.08 * controller.get_zoom(), -direction.x / 10, 1));

        float d = glm::distance(current_eye_, new_eye);
        current_eye_ = d < 0.001f ? current_eye_ : current_eye_ + (new_eye - current_eye_) / 20.f;

        auto view = glm::lookAt(current_eye_, look_at, up);
        auto projection = glm::perspective(glm::radians(80.f), ratio, 0.001f, 25.f);
        return {view, projection};
    }

};