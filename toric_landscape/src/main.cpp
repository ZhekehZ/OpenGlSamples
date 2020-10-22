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
#include <src/controls/controller.h>

#include "src/controls/camera.h"
#include "ogl_imgui_utils.h"
#include "config.h"

int main(int, char **) {
    auto window = init_OGL(CAPTION);
    if (!window) return 1;
    init_ImGui(window);

    auto [assets, objects, skyboxes, textures, shaders] = get_assets();
    auto [global_far_VP, global_near_P] = get_light_matrices();

    Shader torus_shader  (shaders.get(TOR_SHADER));
    Shader skybox_shader (shaders.get(SKYBOX_SHADER));
    Shader car_shader    (shaders.get(OBJECT_SHADER));
    Shader trivial_shader(shaders.get(TRIVIAL_SHADER));

    Mesh<0> car(objects.get(OBJECT), car_shader);
    Torus<1, 2, 3> torus(
        textures.get_s(HEIGHT_MAP),
        TOR_FST_R, TOR_SND_R, TOR_HEIGHT,
        torus_shader,
        textures.get(TILES),
        TILE1, TILE2, TILE3
    );
    Skybox<4> box(skyboxes.get(SKYBOX), skybox_shader);

    Camera camera(torus);
    Controller controller(torus);

    LightSystem<Shadow<1024>> lights(trivial_shader);
    lights.add(GLOBAL_FAR, global_far_VP);
    lights.add(GLOBAL_NEAR);
    lights.add(DIRECTIONAL1);

    float car_scale = OBJECT_SIZE / car.get_size();
    auto car_model = get_object_global_matrix(car_scale);
    float car_height = car.get_length() / 2 * car_scale;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        float ratio = float(display_w) / float(display_h);
        glEnable(GL_DEPTH_TEST);

        controller.handle_keys();
        controller.update();

        auto trans = torus.get_transformation_to_pos(controller.get_position(), car_height)
                        * controller.get_full_rotation();
        auto model = trans * car_model;

        auto carPosition = glm::vec3(model * glm::vec4(0, 0, 0, 1));
        auto globalNearLightView = glm::lookAt(
            carPosition + glm::vec3(0, 0, 0.1), carPosition,glm::vec3(0, 1, 0)
        );

        lights[GLOBAL_NEAR].update_vp(global_near_P * globalNearLightView);
        lights[DIRECTIONAL1].update_vp(
                get_directional_light_VP(trans, controller.get_position(), controller.get_direction()));

        auto [view, projection] = camera.get_VP(car_model, car_height, controller, ratio);
        glm::mat4 vp = projection * view;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        lights.render_all<
            to_slot<GLOBAL_NEAR,  5>,
            to_slot<GLOBAL_FAR,   6>,
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