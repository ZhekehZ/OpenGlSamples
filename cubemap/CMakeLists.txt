cmake_minimum_required(VERSION 3.0)
project(cubemap CXX)

set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

# CONFIG option is important so that CMake doesnt search for modules into the default modules directory
find_package(imgui CONFIG)
find_package(glfw CONFIG)
find_package(glew CONFIG)
find_package(fmt CONFIG)
find_package(glm CONFIG)
find_package(stb CONFIG)
find_package(tinyobjloader CONFIG)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCES
        src/*.cpp src/*.h
        src/buffer_utils/*.h
        src/shader_utils/*.cpp src/shader_utils/*.h
        assets/shaders/*.vs assets/shaders/*.fs
        bindings/*.cpp bindings/*.hpp)

add_executable(cubemap ${SOURCES})

add_custom_command(TARGET cubemap
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/assets ${PROJECT_BINARY_DIR}/assets
)

target_compile_definitions(cubemap PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW)
target_link_libraries(cubemap
                        tinyobjloader::tinyobjloader
                        imgui::imgui GLEW::glew_s
                        glfw::glfw fmt::fmt glm::glm stb::stb)