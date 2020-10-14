#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/matrix_interpolation.hpp>

#include "imgui.h"

#include "shader_utils/opengl_shader.h"
#include "buffer_utils/cube_data.h"
#include "buffer_utils/texture_loader.h"
#include "buffer_utils/init_ogl_buffer.h"
#include "ogl_imgui_utils.h"
#include "file_utils.h"

#include "mesh.h"
#include "skybox.h"
#include "torus.h"
#include "camera.h"


int main(int, char **) {
    auto window = init_OGL("Viewer");
    if (!window) return 1;
    init_ImGui(window);

    Resource assets;
    Resource objects_dir = assets.get("objects");
    Resource skyboxes_dir = assets.get("skyboxes");
    Resource shaders_dir = assets.get("shaders");

    shader_t torus_shader(shaders_dir.get_s("object"));
    shader_t skybox_shader(shaders_dir.get_s("skybox"));
    shader_t car_shader(shaders_dir.get_s("objectCar"));

    Mesh<0> car(assets.get("objects", "CyberpunkDeLorean.obj"), car_shader);
    Torus<1, 2, 3> torus(assets.get_s("textures", "height_map.jpg"), 4, 1,
                         torus_shader, assets.get("textures", "tiles"));
    Skybox<4> box(assets.get("skyboxes", "Blue"), skybox_shader);

    Camera camera(torus);

    float car_scale = 0.06f / car.get_size();

    auto car_model = glm::orientation(glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)) *
                     glm::scale(glm::vec3(car_scale, car_scale, car_scale));



    float zoom = 0.1f;
    float velocity = 0;
    glm::vec2 direction{0, 1};
    float dir_angle = 0;
    glm::vec2 position{0, 0.5};

    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        float ratio = float(display_w) / float(display_h);

        glViewport(0, 0, display_w, display_h);


        glClear(unsigned(GL_COLOR_BUFFER_BIT) | unsigned(GL_DEPTH_BUFFER_BIT));

        if (ImGui::IsKeyPressed(GLFW_KEY_W)) {
            velocity += 0.0007f;
        }
        if (ImGui::IsKeyPressed(GLFW_KEY_S)) {
            velocity -= 0.0007f;
        }
        velocity /= 1.2f;
        velocity = std::max(std::min(velocity, .005f), -.005f);
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

        position += direction * (velocity * glm::vec2(torus.get_ratio(), 1.0)) / 6.f;

        auto rotated_model = glm::rotate(-dir_angle, glm::vec3(0, 1, 0)) * car_model;
        auto trans = torus.get_transformation_to_pos(position, car.get_length() / 2 * car_scale);
        auto model = trans * torus.get_rotation(position.x, position.y) * rotated_model;

        auto vp = camera.get_VP(trans * rotated_model, position, direction, ratio);
        auto mvp = vp * model;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        torus.draw(vp);
        car.draw(mvp, model);
        box.draw(vp);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    dispose_ImGui();
    dispose_OGL(window);
    return 0;
}
