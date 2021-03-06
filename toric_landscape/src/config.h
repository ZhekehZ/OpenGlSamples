#pragma once

#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <src/io/resource.h>

inline const double MAX_FPS = 60;
inline const std::chrono::milliseconds frame_time(static_cast<long>(1000.0 / MAX_FPS));

inline const char * ASSETS_PATH = "assets";

inline const char * CAPTION    = "TOR";
inline const char * OBJECT     = "CyberpunkDeLorean.obj";
inline const char * HEIGHT_MAP = "height_map.jpg";
inline const char * DETAIL     = "detail.jpg";
inline const char * TILES      = "tiles";
inline const char * TILE1      = "p2.png";
inline const char * TILE2      = "p1.jpg";
inline const char * TILE3      = "p3.jpg";
inline const char * SKYBOX     = "galaxy";

inline const char * TOR_SHADER     = "torus";
inline const char * OBJECT_SHADER  = "object";
inline const char * SKYBOX_SHADER  = "skybox";
inline const char * TRIVIAL_SHADER = "trivial";

inline const float TOR_FST_R = 2.0f;
inline const float TOR_SND_R = 0.25f;
inline const float TOR_HEIGHT = 0.17f;
inline const float OBJECT_SIZE = 0.05f;

inline auto get_object_global_matrix(float scale) {
    return glm::orientation(glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)) *
           glm::scale(glm::vec3(scale, scale, scale));
}

inline auto get_light_matrices() {
    auto global_far_VP =
        glm::ortho(-2.5f, 2.5f, -2.5f, 2.5f, 0.5f, 1.5f) *
        glm::lookAt<float>(
            glm::vec3(0, 0, 1),
            glm::vec3(0, 0, 0),
            glm::vec3(0, 1, 0)
        );
    auto global_near_P = glm::ortho(-0.2f, 0.2f, -0.2f, 0.2f, 0.02f, 0.4f);
    return std::make_tuple(global_far_VP, global_near_P);
}

inline auto get_directional_light_VP(
    glm::mat4 const & model,
    glm::vec2 const & position,
    glm::vec2 const & direction
) {
    glm::vec3 look_at = glm::vec3(model * glm::vec4(0, 0, 0.1, 1));
    glm::vec3 up = glm::vec3(model * glm::vec4(0, 1, 0, 0));
    glm::vec3 eye = glm::vec3(model * glm::vec4(0, 0.01, 0, 1));

    auto view = glm::lookAt(eye, look_at, up);
    auto projection = glm::perspective(glm::radians(30.f), 1.f, 0.01f, 0.5f);
    return projection * view;
}

inline auto get_assets() {
    Resource ASSETS(ASSETS_PATH);
    Resource OBJECTS = ASSETS.get("objects");
    Resource SKYBOXES = ASSETS.get("skyboxes");
    Resource TEXTURES = ASSETS.get("textures");
    Resource SHADERS = ASSETS.get("shaders");
    return std::make_tuple(ASSETS, OBJECTS, SKYBOXES, TEXTURES, SHADERS);
}

inline glm::mat4 get_lightVP(
    glm::mat4 const& model,
    glm::vec2 const& position,
    glm::vec2 const& direction
) {
    glm::vec3 look_at = glm::vec3(model * glm::vec4(0, 0, 1, 1));
    glm::vec3 up = glm::vec3(model * glm::vec4(0, 1, 0, 0));
    glm::vec3 eye = glm::vec3(model * glm::vec4(0, 0.01, 0.03, 1));

    auto view = glm::lookAt(eye, look_at, up);
    auto projection = glm::perspective(glm::radians(30.f), 1.f, 0.001f, 0.2f);
    return projection * view;
}