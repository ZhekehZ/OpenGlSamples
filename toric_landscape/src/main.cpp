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

static constexpr unsigned DW = (1u << 12u);
static constexpr unsigned DH = (1u << 12u);


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
    shader_t trivial_shader(shaders_dir.get_s("trivial"));

    Mesh<0> car(assets.get("objects", "CyberpunkDeLorean.obj"), car_shader);
    Torus<1, 2, 3> torus(assets.get_s("textures", "height_map.jpg"), 2, 0.25, 0.2,
                         torus_shader, assets.get("textures", "tiles"));
    Skybox<4> box(assets.get("skyboxes", "Blue"), skybox_shader);

    Camera camera(torus);

    float car_scale = 0.05f / car.get_size();

    auto car_model = glm::orientation(glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)) *
                     glm::scale(glm::vec3(car_scale, car_scale, car_scale));

    GLuint shadow_map, depth_buffer;
    // Texture creation
    glGenTextures(1, &shadow_map);
    glBindTexture(GL_TEXTURE_2D, shadow_map);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, DW, DH, 0,
                    GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
    // Buffer creation
    glGenFramebuffers(1, &depth_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, depth_buffer);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadow_map, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Framebuffer initialization error");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);





    float zoom = 0.1f;
    float velocity = 0;
    glm::vec2 direction{0, 1};
    float dir_angle = 0;
    glm::vec2 position{0, 0.5};



    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
//        display_w = 800, display_h = 600;
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

        auto rotated_model = glm::rotate(-dir_angle, glm::vec3(0, 1, 0)) * car_model;
        auto trans = torus.get_transformation_to_pos(position, car.get_length() / 2 * car_scale);
        auto model = trans * torus.get_rotation(position) * rotated_model;

        auto [view1, projection1] = camera.get_VP(trans * rotated_model, position, direction, ratio);
//        auto mvp = vp * model;


        static float time = 0;
        time += 0.005f;
//        auto view = glm::lookAt<float>(glm::vec3(model * glm::vec4(0, 0, 0, 1)) + glm::vec3(0, 0, 0.1),
//                                       glm::vec3(model * glm::vec4(0, 0, 0, 1)),
//                                       glm::vec3(0, 1, 0));
        auto view = glm::lookAt<float>( glm::vec3(0, 0, 1),
                                       glm::vec3(0, 0, 0),
                                       glm::vec3(0, 1, 0));
        auto projection = glm::ortho(-2.5f, 2.5f, -2.5f, 2.5f, 0.000001f, 2.f);
        auto smvp = projection * view * model;
        auto svp = projection * view;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        glBindFramebuffer(GL_FRAMEBUFFER, depth_buffer);
        glViewport(0, 0, DW, DH);
        glClear(GL_DEPTH_BUFFER_BIT);

        trivial_shader.use();
        trivial_shader.set_uniform("u_mvp", glm::value_ptr(svp));
        torus.draw<true>(svp, svp);
        trivial_shader.set_uniform("u_mvp", glm::value_ptr(smvp));
        car.draw<true>(smvp, model, model, model);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, display_w, display_h);
        glClear(unsigned(GL_COLOR_BUFFER_BIT) | unsigned(GL_DEPTH_BUFFER_BIT));


        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, shadow_map);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

//        vp = svp;
//        mvp = smvp;

        glm::mat4 vp = projection1 * view1;

        torus.draw(vp, svp);
        car.draw(model, view1, projection1, smvp);
        box.draw(vp);


        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    dispose_ImGui();
    dispose_OGL(window);
    return 0;
}
