#pragma once

#include <glm/glm.hpp>

inline const char * CAPTION    = "TOR";
inline const char * OBJECT     = "CyberpunkDeLorean.obj";
inline const char * HEIGHT_MAP = "height_map.jpg";
inline const char * TILES      = "tiles";
inline const char * TILE1      = "p1.jpg";
inline const char * TILE2      = "p13.jpg";
inline const char * TILE3      = "p11.jpg";
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
        glm::ortho(-2.5f, 2.5f, -2.5f, 2.5f, 0.000001f, 2.f) *
        glm::lookAt<float>(
            glm::vec3(0, 0, 1),
            glm::vec3(0, 0, 0),
            glm::vec3(0, 1, 0)
        );
    auto global_near_P = glm::ortho(-0.2f, 0.2f, -0.2f, 0.2f, 0.000001f, 0.5f);
    return std::make_tuple(global_far_VP, global_near_P);
}

inline auto get_directional_light_VP(
    glm::mat4 const & model,
    glm::vec2 const & position,
    glm::vec2 const & direction
) {
    static float d = 0;
    if (ImGui::IsKeyPressed(GLFW_KEY_R)) {
        d += 0.007f;
        std::cout << d << std::endl;
    }
    if (ImGui::IsKeyPressed(GLFW_KEY_T)) {
        d -= 0.007f;
        std::cout << d << std::endl;
    }

    glm::vec3 look_at = glm::vec3(model * glm::vec4(0, 0, 0.1 + d, 1));
    glm::vec3 up = glm::vec3(model * glm::vec4(0, 1, 0, 0));
    glm::vec3 eye = glm::vec3(model * glm::vec4(0, 0.01 + d, 0, 1));

    glBegin(GL_TRIANGLES);
    glVertex3fv(glm::value_ptr(eye));
    glVertex3fv(glm::value_ptr(look_at));
    glVertex3fv(glm::value_ptr(eye + up));
    glEnd();

    auto view = glm::lookAt(eye, look_at, up);
    auto projection = glm::perspective(glm::radians(30.f), 1.f, 0.01f, 0.5f);
    return projection * view;
}

inline auto get_assets() {
    Resource ASSETS;
    Resource OBJECTS = ASSETS.get("objects");
    Resource SKYBOXES = ASSETS.get("skyboxes");
    Resource TEXTURES = ASSETS.get("textures");
    Resource SHADERS = ASSETS.get("shaders");
    return std::make_tuple(ASSETS, OBJECTS, SKYBOXES, TEXTURES, SHADERS);
}
