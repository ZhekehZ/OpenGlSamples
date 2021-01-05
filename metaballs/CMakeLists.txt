cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 17)

project(metaballs CXX)

set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
set(RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

find_package(imgui CONFIG)
find_package(glfw CONFIG)
find_package(glfw3 CONFIG)
find_package(glew CONFIG)
find_package(fmt CONFIG)
find_package(glm CONFIG)
find_package(stb CONFIG)

set(src_dirs data shader utils bindings)
file(GLOB SOURCES src/*.cpp src/*.h assets/shaders/skybox/*.shader assets/shaders/metaballs/*.shader)
foreach(dir ${src_dirs})
    file(GLOB ADD_SOURCES  src/${dir}/*.cpp src/${dir}/*.h)
    list(APPEND SOURCES ${ADD_SOURCES})
endforeach()
add_executable(${PROJECT_NAME} ${SOURCES})

add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/assets
                                               ${CMAKE_BINARY_DIR}/${ASSETS_TARGET_DIR}/assets
)

add_compile_definitions(STB_IMAGE_IMPLEMENTATION)

target_compile_definitions(${PROJECT_NAME} PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW)
target_link_libraries(${PROJECT_NAME} imgui::imgui GLEW::glew_s glfw::glfw fmt::fmt glm::glm stb::stb)
