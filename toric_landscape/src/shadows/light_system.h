#pragma once

#include <map>
#include <glm/glm.hpp>
#include "shadow_map.h"

enum light_t {
    GLOBAL_FAR, GLOBAL_NEAR, DIRECTIONAL1
};

template <light_t Type, std::size_t Slot> struct to_slot {
    static constexpr light_t type = Type;
    static constexpr std::size_t slot = Slot;
};

template <typename Shadow>
class LightSystem : public std::map<light_t, Shadow> {
    Shader shader_;

public:
    explicit LightSystem(const Shader &shader) : shader_(shader) {}

    void add(light_t t, glm::mat4 const & mat) {
        LightSystem::emplace(t, Shadow(mat, shader_));
    }

    void add(light_t t) {
        LightSystem::emplace(t, Shadow(shader_));
    }

    template <typename ... TypeToSlot>
    void render_all(std::function<void(detail::Drawer const &)> const & func) {
        (((*this)[TypeToSlot::type].render<TypeToSlot::slot>(func)), ...);
    }
};
