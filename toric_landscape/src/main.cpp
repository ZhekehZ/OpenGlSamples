#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/transform.hpp>
#include <imgui.h>

#include <src/shaders/opengl_shader.h>
#include <src/io/resource.h>
#include <src/shapes/mesh.h>
#include <src/shapes/skybox.h>
#include <src/shapes/torus.h>
#include <src/shadows/shadow_map.h>
#include <src/shadows/light_system.h>

#include "src/controls/camera.h"
#include "ogl_imgui_utils.h"

int main(int, char **) {
    auto window = init_OGL("Viewer");
    if (!window) return 1;
    init_ImGui(window);

    Resource assets;
    Resource objects_dir = assets.get("objects");
    Resource skyboxes_dir = assets.get("skyboxes");
    Resource shaders_dir = assets.get("shaders");

    shader_t torus_shader(shaders_dir.get("object"));
    shader_t skybox_shader(shaders_dir.get("skybox"));
    shader_t car_shader(shaders_dir.get("objectCar"));
    shader_t trivial_shader(shaders_dir.get("trivial"));

    Mesh<0> car(assets.get("objects", "CyberpunkDeLorean.obj"), car_shader);
    Torus<1, 2, 3> torus(assets.get_s("textures", "height_map.jpg"), 2, 0.25f, 0.2f,
                         torus_shader, assets.get("textures", "tiles"),
                         "p1.jpg", "p3.jpg", "p2.jpg");
    Skybox<4> box(assets.get("skyboxes", "galaxy"), skybox_shader);

    Camera camera(torus);

    float car_scale = 0.05f / car.get_size();

    auto car_model = glm::orientation(glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)) *
                     glm::scale(glm::vec3(car_scale, car_scale, car_scale));

    auto globalFarLightView = glm::lookAt<float>(
            glm::vec3(0, 0, 1),
            glm::vec3(0, 0, 0),
            glm::vec3(0, 1, 0)
    );
    auto globalFarLightProjection = glm::ortho(-2.5f, 2.5f, -2.5f, 2.5f, 0.000001f, 2.f);
    auto globalNearLightProjection = glm::ortho(-0.2f, 0.2f, -0.2f, 0.2f, 0.000001f, 0.5f);

    LightSystem<Shadow<1024>> lights(trivial_shader);
    lights.add(GLOBAL_FAR, globalFarLightProjection * globalFarLightView);
    lights.add(GLOBAL_NEAR);
    lights.add(DIRECTIONAL1);

    float zoom = 0.1f;
    float velocity = 0;
    glm::vec2 direction{0, 1};
    float dir_angle = 0;
    glm::vec2 position{0, 0.5};

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        float ratio = float(display_w) / float(display_h);
        glEnable(GL_DEPTH_TEST);


        if (ImGui::IsKeyPressed(GLFW_KEY_W)) {
            velocity += 0.0007f;
        }
        if (ImGui::IsKeyPressed(GLFW_KEY_S)) {
            velocity -= 0.0007f;
        }
        velocity /= 1.2f;
        velocity = std::max(std::min(velocity, .5f), -.005f);
        if (abs(velocity) < 0.0002) velocity = 0;

        if (ImGui::IsKeyPressed(GLFW_KEY_D)) {
            dir_angle += 0.1f;
        }
        if (ImGui::IsKeyPressed(GLFW_KEY_A)) {
            dir_angle -= 0.1f;
        }

        direction = {cos(dir_angle), sin(dir_angle)};

        auto wheel = ImGui::GetIO().MouseWheel;
        if (abs(wheel) > 0.1) {
            zoom *= wheel > 0 ? 1.2f : 1/1.2f;
        }

        position += direction * (velocity * glm::vec2(torus.get_ratio(), 1.0)) / 2.f;

        auto rotation = glm::rotate(-dir_angle, glm::vec3(0, 1, 0));
        auto full_rotation = torus.get_rotation(position) * rotation;
        auto rotated_model = rotation * car_model;
        auto trans = torus.get_transformation_to_pos(position, car.get_length() / 2 * car_scale);
        auto model = trans * torus.get_rotation(position) * rotated_model;

        auto carPosition = glm::vec3(model * glm::vec4(0, 0, 0, 1));
        auto globalNearLightView = glm::lookAt(
            carPosition + glm::vec3(0, 0, 0.1), carPosition,glm::vec3(0, 1, 0)
        );

        lights[GLOBAL_NEAR].update_vp(globalNearLightProjection * globalNearLightView);
        lights[DIRECTIONAL1].update_vp(Camera::get_lightVP(trans * full_rotation, position, direction));

        auto [view, projection] = camera.get_VP(trans * rotated_model, position, direction, ratio);
        glm::mat4 vp = projection * view;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        lights.render_all<
            to_slot<GLOBAL_NEAR, 5>,
            to_slot<GLOBAL_FAR, 6>,
            to_slot<DIRECTIONAL1, 7>
        >([&](auto const & draw_callback) {
            draw_callback(torus);
            draw_callback(car, model);
        });

        glViewport(0, 0, display_w, display_h);
        glClear(unsigned(GL_COLOR_BUFFER_BIT) | unsigned(GL_DEPTH_BUFFER_BIT));

        torus.draw(vp, lights);
        car.draw(model, view, projection, lights);
        box.draw(vp);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    dispose_ImGui();
    dispose_OGL(window);
    return 0;
}
