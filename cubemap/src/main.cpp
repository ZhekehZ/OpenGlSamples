#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"

#include "shader_utils/opengl_shader.h"
#include "buffer_utils/cube_data.h"
#include "buffer_utils/texture_loader.h"
#include "buffer_utils/obj_loader.h"
#include "buffer_utils/init_ogl_buffer.h"
#include "ogl_imgui_utils.h"
#include "file_utils.h"

#include "mesh.h"

int main(int, char **) {
    auto window = init_OGL("Viewer");
    if (!window) return 1;
    init_ImGui(window);

    Resource assets;
    Resource objects_dir = assets.get("objects");
    Resource skyboxes_dir = assets.get("skyboxes");
    Resource shaders_dir = assets.get("shaders");

    std::vector<Mesh> objects;
    std::string combobox_options_object;
    for (auto const & p : objects_dir.iterate()) {
        auto const & path = p.path();
        if (path.extension() == ".obj") {
            combobox_options_object.append(path.filename().string()).push_back('\0');
            objects.push_back(load_OBJ(path));
        }
    }
    GLuint skybox = init_ogl_buffer(cubeV, cubeVSize, cubeF, cubeFSize);

    std::vector<GLuint> skybox_textures;
    std::string combobox_options_skybox;
    for (const auto & p : skyboxes_dir.iterate()) {
        Resource box = p.path();
        combobox_options_skybox.append(p.path().filename().string()).push_back('\0');
        skybox_textures.push_back(
            load_cube_texture({
            box.get_s("sb_right.jpg"),
            box.get_s("sb_left.jpg"),
            box.get_s("sb_up.jpg"),
            box.get_s("sb_down.jpg"),
            box.get_s("sb_front.jpg"),
            box.get_s("sb_back.jpg")
        }));
    }

    shader_t objectShader(shaders_dir.get_s("object.vs"),
                          shaders_dir.get_s("object.fs"));
    shader_t skyboxShader(shaders_dir.get_s("skybox.vs"),
                          shaders_dir.get_s("skybox.fs"));

    float alpha = 0.0f, beta = 0.0f;
    float zoom = 0.1f;

    static float reflectivity = 0.5f;
    static float color_intensity = 0.5f;
    static float refraction_value = 0.6f;

    int selected_skybox = 0;
    int selected_object = 0;

    bool pre_test = false;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.8f, 0.55f, 0.60f, 0.00f);

        glDepthFunc(GL_LEQUAL);
        glEnable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glEnable(GL_ALPHA_TEST);

        glm::vec4 camera(0, 0, 1, 1);

        if (!ImGui::IsAnyWindowFocused()) {
            auto[dx, dy] = ImGui::GetMouseDragDelta();
            ImGui::ResetMouseDragDelta();

            alpha = std::max(std::min(alpha - dy / 4, 88.f), -88.f);
            beta -= dx / 4;
        }

        auto rotH = glm::rotate(glm::mat4(1), glm::radians(beta), glm::vec3(0, 1, 0));
        auto OX = glm::vec3(rotH * glm::vec4(1, 0, 0, 1));
        camera = rotH * camera;
        camera = glm::rotate(glm::mat4(1), glm::radians(alpha), OX) * camera;

        auto wheel = ImGui::GetIO().MouseWheel;
        if (abs(wheel) > 0.1) {
            zoom *= wheel > 0 ? 1.1f : 1/1.1f;
        }

        auto model = glm::scale(glm::vec3(zoom, zoom, zoom));
        auto model_normal = glm::inverse(glm::transpose(model));

        auto view = glm::lookAt<float>(
                glm::vec3(camera.x, camera.y, camera.z),
                glm::vec3(0, 0, 0),
                glm::vec3(0, 1, 0)
        );
        auto projection = glm::perspective<float>(90, float(display_w) / float(display_h), 0.1f, 100.0f);

        auto mvp = projection * view * model;
        auto vp = projection * view;

        glClear(unsigned(GL_COLOR_BUFFER_BIT) | unsigned(GL_DEPTH_BUFFER_BIT));

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        ImGui::Begin("Settings");
        ImGui::SliderFloat("Refraction -- Reflection", &reflectivity, 0, 1);
        ImGui::SliderFloat("Color intensity", &color_intensity, 0, 1);
        ImGui::SliderFloat("Refraction coefficient", &refraction_value, 0, 1);
        ImGui::Spacing();
        ImGui::Combo("Object", &selected_object, combobox_options_object.c_str());
        ImGui::Combo("Skybox", &selected_skybox, combobox_options_skybox.c_str());
        ImGui::Spacing();
        ImGui::Checkbox("Enable pre-z-test", &pre_test);
        ImGui::End();

        // Render object
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_textures[selected_skybox]);

        objectShader.use();
        objectShader.set_uniform("u_model", glm::value_ptr(model));
        objectShader.set_uniform("u_model_normal", glm::value_ptr(model_normal));
        objectShader.set_uniform("u_mvp", glm::value_ptr(mvp));
        objectShader.set_uniform("u_camera", camera.x, camera.y, camera.z);
        objectShader.set_uniform("u_skybox", 1);
        objectShader.set_uniform("u_reflectivity", reflectivity);
        objectShader.set_uniform("u_color_intensity", color_intensity);
        objectShader.set_uniform("u_refraction_value", refraction_value);

        objects[selected_object].draw(objectShader, pre_test);

        // Render skybox
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_textures[selected_skybox]);
        skyboxShader.use();
        skyboxShader.set_uniform("u_mvp", glm::value_ptr(vp));
        skyboxShader.set_uniform("u_skybox", 1);

        glBindVertexArray(skybox);
        glDrawElements(GL_TRIANGLES, cubeFSize, GL_UNSIGNED_INT, nullptr);

        glBindVertexArray(0);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    dispose_ImGui();
    dispose_OGL(window);
    return 0;
}
