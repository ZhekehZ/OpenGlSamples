cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 17)

project(metaballs CXX)

set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

# CONFIG option is important so that CMake doesnt search for modules into the default modules directory
find_package(imgui CONFIG)
find_package(glfw CONFIG)
find_package(glew CONFIG)
find_package(fmt CONFIG)
find_package(glm CONFIG)
find_package(stb CONFIG)

add_executable( ${PROJECT_NAME}
                main.cpp
                opengl_shader.cpp
                opengl_shader.h
                bindings/imgui_impl_glfw.cpp
                bindings/imgui_impl_opengl3.cpp
                bindings/imgui_impl_glfw.h
                bindings/imgui_impl_opengl3.h
        assets/vertex.shader
        assets/fragment.shader
        assets/geometry.shader)

add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/assets
        ${CMAKE_BINARY_DIR}/${ASSETS_TARGET_DIR}/assets
        )

add_compile_definitions(STB_IMAGE_IMPLEMENTATION)

target_compile_definitions(${PROJECT_NAME} PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW)
target_link_libraries(${PROJECT_NAME} imgui::imgui GLEW::glew_s glfw::glfw fmt::fmt glm::glm stb::stb)
