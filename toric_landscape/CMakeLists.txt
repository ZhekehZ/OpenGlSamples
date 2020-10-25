cmake_minimum_required(VERSION 3.15)
project(torus CXX)

set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
set(RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if (MSVC)
    set(ASSETS_TARGET_DIR Release)
endif ()

# CONFIG option is important so that CMake doesnt search for modules into the default modules directory
find_package(glfw CONFIG)
find_package(glew CONFIG)
find_package(fmt CONFIG)
find_package(glm CONFIG)
find_package(stb CONFIG)
find_package(tinyobjloader CONFIG)

set(CMAKE_CXX_STANDARD 17)

set(src_dirs shadows shaders shapes io controls)

file(GLOB SOURCES src/*.cpp src/*.h assets/shaders/*.vs assets/shaders/*.fs)

foreach(dir ${src_dirs})
    file(GLOB ADD_SOURCES  src/${dir}/*.cpp src/${dir}/*.h)
    list(APPEND SOURCES ${ADD_SOURCES})
endforeach()

foreach(src ${SOURCES})
    message("SRC ${src}")
endforeach()

add_executable(${PROJECT_NAME} ${SOURCES})

add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/assets 
                                               ${CMAKE_BINARY_DIR}/${ASSETS_TARGET_DIR}/assets
)

add_compile_definitions(STB_IMAGE_IMPLEMENTATION)
add_compile_definitions(TINYOBJLOADER_IMPLEMENTATION)

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME}
                        tinyobjloader::tinyobjloader GLEW::glew_s
                        glfw::glfw fmt::fmt glm::glm stb::stb)