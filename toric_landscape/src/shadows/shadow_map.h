#pragma once

#include <stdexcept>
#include <functional>
#include <GLFW/glfw3.h>
#include <glm/gtx/transform.hpp>
#include <src/shaders/opengl_shader.h>

namespace detail {
    struct Drawer {
        glm::mat4 * vp = nullptr;
        Shader * shader = nullptr;

        template <typename T>
        void operator()(T & obj, glm::mat4 const & model) const {
            auto mat = *vp * model;
            auto mvp = glm::value_ptr(mat);
            shader->set_uniform("u_mvp", mvp);
            obj.draw();
        }
        template <typename T>
        void operator()(T & obj) const {
            shader->set_uniform("u_mvp", glm::value_ptr(*vp));
            obj.draw();
        }
    };
}

template <std::size_t Width, std::size_t Height=Width>
class Shadow {
    GLuint buffer_{};
    GLuint texture_{};
    glm::mat4 vp_ = glm::mat4(1);
    Shader shader_{};
    std::size_t current_slot_ = -1;

public:
    Shadow() = default;
    explicit Shadow(Shader const & shader) : Shadow(glm::mat4(1), shader) {}
    Shadow(glm::mat4 const & vp, Shader const & shader)
        : vp_(vp)
        , shader_(shader)
    {
        glGenTextures(1, &texture_);
        glBindTexture(GL_TEXTURE_2D, texture_);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, Width, Height, 0,
                     GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);

        glGenFramebuffers(1, &buffer_);
        glBindFramebuffer(GL_FRAMEBUFFER, buffer_);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture_, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            throw std::runtime_error("Framebuffer initialization error");
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void update_vp(glm::mat4 const & mvp) {
        vp_ = mvp;
    }

    glm::mat4 get_MVP(glm::mat4 const & model) const {
        return vp_ * model;
    }

    glm::mat4 get_VP() const {
        return vp_;
    }

    int get_current_slot() const {
        return int(current_slot_);
    }

    void render(std::function<void(detail::Drawer const &)> const & func, std::size_t textureSlot) {
        current_slot_ = textureSlot;
        detail::Drawer drawer{&vp_, &shader_};

        glBindFramebuffer(GL_FRAMEBUFFER, buffer_);
        glViewport(0, 0, Width, Height);
        glClear(GL_DEPTH_BUFFER_BIT);
        shader_.use();
        func(drawer);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glActiveTexture(GL_TEXTURE0 + textureSlot);
        glBindTexture(GL_TEXTURE_2D, texture_);
    }

};
