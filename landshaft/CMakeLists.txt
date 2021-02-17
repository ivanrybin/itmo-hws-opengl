cmake_minimum_required(VERSION 3.0)

project(Landschaft CXX)

set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

# CONFIG option is important so that CMake doesnt search for modules into the default modules directory
find_package(imgui CONFIG)
find_package(glfw CONFIG)
find_package(glew CONFIG)
find_package(fmt CONFIG)
find_package(glm CONFIG)
find_package(stb CONFIG)

add_executable(
        Landschaft
        main.cpp
        project/camera/camera.h
        project/textures/texture.cpp
        project/textures/texture.h
        project/models/model.cpp
        project/models/model.h
        project/models/mesh.cpp
        project/models/mesh.h
        project/world/skybox.h
        project/world/landschaft.h
        project/world/landschaft.cpp
        project/world/water.cpp
        project/world/water.h
        project/world/water_fb.cpp
        project/world/water_fb.h
        project/framebuffer/frame_buffer.cpp
        project/framebuffer/frame_buffer.h
        project/gui/gui_handler.cpp
        project/gui/gui_handler.h
        project/models/scene_object.cpp
        project/models/scene_object.h
        assets/skybox/VS.glsl
        assets/skybox/FS.glsl
        assets/model/FS.glsl
        assets/model/VS.glsl
        assets/hmap/VS.glsl
        assets/hmap/FS.glsl
        assets/water/VS.glsl
        assets/water/FS.glsl
        assets/gui/VS.glsl
        assets/gui/FS.glsl
        assets/light/VS.glsl
        assets/light/FS.glsl
        project/shaders/opengl_shader.cpp
        project/shaders/opengl_shader.h
        bindings/imgui_impl_glfw.cpp
        bindings/imgui_impl_opengl3.cpp
        bindings/imgui_impl_glfw.h
        bindings/imgui_impl_opengl3.h
        bindings/stb_image.h
        bindings/tiny_obj_loader.h
)

#add_custom_command(TARGET Landschaft
#    POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy assets/skybox/FS.glsl ${PROJECT_BINARY_DIR}
#    COMMAND ${CMAKE_COMMAND} -E copy assets/skybox/VS.glsl ${PROJECT_BINARY_DIR}
#)

target_compile_definitions(Landschaft PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW)

target_link_libraries(Landschaft imgui::imgui GLEW::glew_s glfw::glfw fmt::fmt glm::glm)
