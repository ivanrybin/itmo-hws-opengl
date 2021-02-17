#include <iostream>
#include <vector>
#include <chrono>

#include <fmt/format.h>

#include <GL/glew.h>

// Imgui + bindings
#include "imgui.h"
#include "bindings/imgui_impl_glfw.h"
#include "bindings/imgui_impl_opengl3.h"

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

// Math constant and routines for OpenGL interop
#include <glm/gtc/constants.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "project/shaders/opengl_shader.h"

#include "project/textures/texture.h"
#include "project/camera/camera.h"
#include "project/models/model.h"
#include "project/world/skybox.h"
#include "project/world/landschaft.h"
#include "project/world/water.h"
#include "project/framebuffer/frame_buffer.h"
#include "project/gui/gui_handler.h"
#include "project/models/scene_object.h"
#include "project/world/water_fb.h"

// configuration -------------------------------------------------------------------------------------------------------

const unsigned int WIN_WIDTH  = 1280;
const unsigned int WIN_HEIGHT =  720;

float FOV = 45.0f;

struct CONFIG {
    const std::string MODEL_VS = "../assets/model/VS.glsl";
    const std::string MODEL_FS = "../assets/model/FS.glsl";

    const std::string SKYBOX_VS = "../assets/skybox/VS.glsl";
    const std::string SKYBOX_FS = "../assets/skybox/FS.glsl";

    const std::string BOAT_VS = "../assets/boat/VS.glsl";
    const std::string BOAT_FS = "../assets/boat/FS.glsl";

    const std::string LIGHTHOUSE_VS = "../assets/lighthouse/VS.glsl";
    const std::string LIGHTHOUSE_FS = "../assets/lighthouse/FS.glsl";

    const std::string BACKPACK_VS = "../assets/backpack/VS.glsl";
    const std::string BACKPACK_FS = "../assets/backpack/FS.glsl";

    const std::string HMAP_VS = "../assets/hmap/VS.glsl";
    const std::string HMAP_FS = "../assets/hmap/FS.glsl";

    const std::string WATER_VS = "../assets/water/VS.glsl";
    const std::string WATER_FS = "../assets/water/FS.glsl";

    const std::string LIGHT_VS = "../assets/light/VS.glsl";
    const std::string LIGHT_FS = "../assets/light/FS.glsl";

    const std::string GUI_VS = "../assets/gui/VS.glsl";
    const std::string GUI_FS = "../assets/gui/FS.glsl";

    const std::string BACKPACK_OBJ = "../textures/backpack/backpack.obj";
    const std::string BACKPACK_DIR = "../textures/backpack/";

    const std::string WATER_DUDV = "../textures/water/dudv_map.png";

    const std::string LIGHTHOUS_OBJ = "../textures/lighthouse/lighthouse.obj";
    const std::string LIGHTHOUS_DIR = "../textures/lighthouse/";

    const std::string BOAT_OBJ = "../textures/boat/PaddleBoat.obj";
    const std::string BOAT_DIR = "../textures/boat/";

    const std::string HEIGHT_MAP = "../textures/map/height.png";
    const std::string MAP_TEXT = "../textures/map/stone2.jpg";

    const std::string SKYBOX_DIR = "../textures/skybox/";
    std::vector<std::string> faces = { "right.jpg", "left.jpg", "top.jpg", "bottom.jpg", "front.jpg", "back.jpg" };


    // models scale
    float LIGHTHOUSE_SCALE  = 3.0f;
    float SKYBOX_SCALE      = 300.0f;
    float BOAT_SCALE        = 0.2f;
    float WATER_SCALE       = 500.0f;
    float HMAP_SCALE        = 60.0f;
};

CONFIG CONF;

// configuration -------------------------------------------------------------------------------------------------------
float delta_time = 0.0f;	// время между текущим кадром и последним кадром
float last_frame = 0.0f;

double x_prev;

double y_prev;

bool is_start = true;

camera::camera scene_camera{};

void mouse_callback(GLFWwindow* window, double x_curr, double y_curr) {
    // пропуск мыши на окне imgui
    if (ImGui::IsAnyWindowHovered() || ImGui::IsAnyItemHovered() || ImGui::IsAnyItemActive()) {
        return;
    }

    if (glfwGetMouseButton(window, 0) != GLFW_PRESS) {
        x_prev = x_curr;
        y_prev = y_curr;
        is_start = false;
        return;
    }

    double x_offset = x_curr - x_prev;
    double y_offset = y_prev - y_curr; // reversed since y-coordinates go from bottom to top

    x_prev = x_curr;
    y_prev = y_curr;

    scene_camera.process_mouse(x_offset, y_offset);
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset) {
    scene_camera.process_mouse_scroll(y_offset);
}

void process_input(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        scene_camera.process_keyboard(camera::FORWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        scene_camera.process_keyboard(camera::BACKWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        scene_camera.process_keyboard(camera::LEFT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        scene_camera.process_keyboard(camera::RIGHT, delta_time);
}

void glfw_error_callback(int error, const char *description)  {
    std::cerr << fmt::format("Glfw Error {}: {}\n", error, description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

GLuint load_cube_map() {
    for (int i = 0; i < CONF.faces.size(); ++i) {
        CONF.faces[i] = CONF.SKYBOX_DIR + CONF.faces[i];
    }
    return my_model::cube_map_loader(CONF.faces);;
}

GLuint init_cube() {
    GLuint VBO, VAO;
    my_model::cube SKY_BOX;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(SKY_BOX.coords), &SKY_BOX.coords, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));

    glBindVertexArray(0);
    return VAO;
}

void draw_cube(GLuint VAO) {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void draw_skybox(GLuint skybox_VAO, GLuint CUBEMAP_TEXT) {
    glActiveTexture(GL_TEXTURE0 + CUBEMAP_TEXT);
    glBindTexture(GL_TEXTURE_CUBE_MAP, CUBEMAP_TEXT);
    draw_cube(skybox_VAO);
}

float square_vertices_1[20] = {
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f,    //   [1]      [0]
        0.5f,  1.0f, 0.0f, 0.0f, 1.0f,    //
        1.0f,  0.5f, 0.0f, 1.0f, 0.0f,    //
        0.5f,  0.5f, 0.0f, 0.0f, 0.0f,    //   [3]      [2]
};

float square_vertices_2[20] = {
        1.0f,  0.5f, 0.0f, 1.0f, 1.0f,    //   [1]      [0]
        0.5f,  0.5f, 0.0f, 0.0f, 1.0f,    //
        1.0f,  0.0f, 0.0f, 1.0f, 0.0f,    //
        0.5f,  0.0f, 0.0f, 0.0f, 0.0f,    //   [3]      [2]
};

enum clip_dist_type {
    NORMAL,
    INVERSE,
    OFF
};

clip_dist_type clip_dist_state = OFF;

int main() {
    // Use GLFW to create a simple window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        return 1;
    }
    // GL 3.3 + GLSL 330
    const char *glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only and MacOS

    // Create window with graphics context
    GLFWwindow *window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "OpenGL -- Rybin Ivan, ITMO JB SE", nullptr, nullptr);
    if (window == nullptr) {
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);    // Enable vsync

    // Initialize GLEW, i.e. fill all possible function pointers for current OpenGL context
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize OpenGL loader!\n";
        return 1;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // MOUSE
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // GUI context ---------------------------------------------------------------------------------------
    gui_handler gui(window, glsl_version);
    GLuint REFL_gui = init_square(square_vertices_1);
    GLuint REFR_gui = init_square(square_vertices_2);

    // MODELS ------------------------------------------------------------------------------------------
    shader_t MODEL_SHADER(CONF.MODEL_VS, CONF.MODEL_FS);
    shader_t SKYBOX_SHADER(CONF.SKYBOX_VS, CONF.SKYBOX_FS);
    shader_t LANDSCHAST_SHADER(CONF.HMAP_VS, CONF.HMAP_FS);
    shader_t WATER_SHADER(CONF.WATER_VS, CONF.WATER_FS);
    shader_t GUI_SHADER(CONF.GUI_VS, CONF.GUI_FS);
    shader_t LIGHT_SHADER(CONF.LIGHT_VS, CONF.LIGHT_FS);

    my_model::model LH_model(CONF.LIGHTHOUS_OBJ, CONF.LIGHTHOUS_DIR);
    my_model::model BT_model(CONF.BOAT_OBJ, CONF.BOAT_DIR);

    scene_object LH_obj{}; // lighthouse
    scene_object BT_obj{}; // boat
    scene_object SB_obj{}; // skybox
    scene_object LS_obj{}; // landschaft
    scene_object LT_obj{}; // light

    // BP_obj.shader = &MODEL_SHADER;
    LH_obj.shader = &MODEL_SHADER;  LH_obj.model = &LH_model;
    BT_obj.shader = &MODEL_SHADER;  BT_obj.model = &BT_model;
    LS_obj.shader = &LANDSCHAST_SHADER;
    SB_obj.shader = &SKYBOX_SHADER;
    LT_obj.shader = &LIGHT_SHADER;

    // SKYBOX ------------------------
    GLuint SKYBOX_VAO = init_cube();
    GLuint SKYBOX_TEXT = load_cube_map();

    // HMAP ---------------------------
    my_model::landschaft HMAP(CONF.HEIGHT_MAP, "", 1);
    my_model::texture    HMAP_TEXT(CONF.MAP_TEXT); HMAP_TEXT.init();

    // WATER ---------------------------
    float WATER_Y = 0.0f;
    my_model::texture dudv_water(CONF.WATER_DUDV); dudv_water.init();
    my_model::water WATER{}; WATER.init();
    glm::vec4 clipPlane = glm::vec4(0, 0, 0, 0);

    // LIGHT_VAO
    GLuint LIGHT_VAO = init_cube();
    LT_obj.cam = &scene_camera;
    LT_obj.scale_ = 2.0f;
    LT_obj.X = -50.0f; LT_obj.Y = 70.0f; LT_obj.Z = 50.0f;
    gui.LX = LT_obj.X, gui.LY = LT_obj.Y, gui.LZ = LT_obj.Z;
    LT_obj.tasks.push_back([&]() {
        LT_obj.X = gui.LX;
        LT_obj.Y = gui.LY;
        LT_obj.Z = gui.LZ;
        LT_obj.need_update = true;
        draw_cube(LIGHT_VAO);
        LT_obj.shader->set_vec3("lightColor", glm::vec3(gui.LR, gui.LG, gui.LB));
    });

    // MODEL INIT - LIGHTHOUSE
    LH_obj.cam = &scene_camera;
    LH_obj.is_light = true;
    LH_obj.is_camera_pos = true;
    LH_obj.scale_   = CONF.LIGHTHOUSE_SCALE;
    LH_obj.X = 29.5;
    LH_obj.Z = -6.5f;
    LH_obj.Y = HMAP.vertices[(int) ((LH_obj.X / 60.0f + 1.0f) * 1024 +
                                    (LH_obj.Z / 60.0f + 1.0f) * 1024)].position.y + 25.0f;
    LH_obj.tasks.push_back([&]() {
        LH_obj.shader->set_vec4("clipPlane", clipPlane);
    });

    // MODEL INIT - BOAT
    BT_obj.cam = &scene_camera;
    BT_obj.is_light = true;
    BT_obj.is_camera_pos = true;
    BT_obj.scale_ = CONF.BOAT_SCALE;
    BT_obj.need_rotate = false;
    BT_obj.X = -10.0f; BT_obj.Y = -0.2f; BT_obj.Z = 50.0f;
    BT_obj.tasks.push_back([&]() {
        BT_obj.shader->set_vec4("clipPlane", clipPlane);
        if (glfwGetTime() - BT_obj.startTime >= 20) {
            BT_obj.startTime = glfwGetTime();
            BT_obj.sign *= -1;
        }
        float delta = glfwGetTime() - last_frame;
        BT_obj.translate(0, 0, 2 * delta * BT_obj.sign);
    });

    // MODEL INIT - SKYBOX
    SB_obj.is_light = true;
    SB_obj.scale_ = CONF.SKYBOX_SCALE;
    SB_obj.tasks.push_back([&](){
        SB_obj.shader->set_uniform<int>("skybox", SKYBOX_TEXT);
        draw_skybox(SKYBOX_VAO, SKYBOX_TEXT);
    });

    // OBJ INIT - LANDSCHAFT
    LS_obj.cam = &scene_camera;
    LS_obj.is_light = true;
    LS_obj.is_camera_pos = true;
    LS_obj.scale_ = CONF.HMAP_SCALE;
    LS_obj.Y = -10.0f;
    LS_obj.tasks.push_back([&]() {
        LS_obj.shader->set_vec4("clipPlane", clipPlane);
        LS_obj.shader->set_uniform<int>("repeats", gui.texture_repeats);
        HMAP.draw(*LS_obj.shader, HMAP_TEXT.id);
    });

    std::function<void(glm::mat4, glm::mat4, clip_dist_type)> render_scene = [&](const glm::mat4& view, const glm::mat4 proj, clip_dist_type clip_type){
        if (clip_type == OFF) {
            clipPlane = glm::vec4(0, 0, 0, 0);
        } else if (clip_type == NORMAL) {
            clipPlane = glm::vec4(0, 1, 0, gui.clip_dist);
        } else if (clip_type == INVERSE) {
            clipPlane = -1.0f * glm::vec4(0, 1, 0, gui.clip_dist);
        }

        glClearColor(gui.R, gui.G, gui.B, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        LT_obj.draw(view, proj, gui); // LIGHT
        LS_obj.draw(view, proj, gui); // LANDSCHAFT
        LH_obj.draw(view, proj, gui); // LIGHTHOUSE
        BT_obj.draw(view, proj, gui); // BOAT
        if (gui.is_skybox) {
            glDepthFunc(GL_LEQUAL);
            SB_obj.draw(glm::mat4(glm::mat3(view)), proj, gui); // SKYBOX
            glDepthFunc(GL_LESS);
        }
    };

    water_fb frame_buf{};

    float move_waves = 0;

    // LZ-TEST
    //----------------------------------------MAIN LOOP---------------------------------------------------
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        delta_time = currentFrame - last_frame;
        last_frame = currentFrame;

        double dummy = 0;
        move_waves += delta_time * 0.1;
        move_waves = (float) std::modf(move_waves, &dummy);

        process_input(window);

        glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);
        glClearColor(gui.R, gui.G, gui.B, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_CLIP_DISTANCE0);
        glEnable(GL_DEPTH_TEST);


        glm::mat4 view = scene_camera.get_view_matrix();
        glm::mat4 proj = glm::perspective<float>(glm::radians(camera::FOV), (float) WIN_WIDTH / (float) WIN_HEIGHT, 0.1f, 1000.0f);

        frame_buf.bindReflectionFrameBuffer(); // --- FRAMEBUFFER START
            float distance = 2 * (scene_camera.position.y - WATER_Y);
            scene_camera.position.y -= distance;
            scene_camera.invert_pitch();
            scene_camera.update_camera_vecs();
            glm::mat4 new_view = scene_camera.get_view_matrix();
            render_scene(new_view, proj, NORMAL);  // SCENE
            scene_camera.position.y += distance;
            scene_camera.invert_pitch();
            scene_camera.update_camera_vecs();
        frame_buf.unbindCurrentFrameBuffer(WIN_WIDTH, WIN_HEIGHT);  // --- FRAMEBUFFER END

        frame_buf.bindRefractionFrameBuffer(); // --- FRAMEBUFFER START
            render_scene(view, proj, INVERSE);  // SCENE
        frame_buf.unbindCurrentFrameBuffer(WIN_WIDTH, WIN_HEIGHT);  // --- FRAMEBUFFER END

        render_scene(view, proj, OFF); // SCENE

        // WATER
        WATER_SHADER.use();
            glm::mat4 water_M = glm::translate(glm::vec3(0.0f, WATER_Y, 0.0f));
            water_M = glm::scale(water_M, glm::vec3(CONF.WATER_SCALE));
            WATER_SHADER.set_mat4("V", view);
            WATER_SHADER.set_mat4("M", water_M);
            WATER_SHADER.set_mat4("P", proj);
            frame_buf.bindReflectionTexture();
            WATER_SHADER.set_uniform<int>("reflectionText", frame_buf.reflectionTexture);
            frame_buf.bindRefractionTexture();
            WATER_SHADER.set_uniform<int>("refractionText", frame_buf.refractionTexture);
            glActiveTexture(GL_TEXTURE0 + dudv_water.id);
            glBindTexture(GL_TEXTURE_2D, dudv_water.id);
            WATER_SHADER.set_uniform<int>("dudv_water", dudv_water.id);
            WATER_SHADER.set_uniform<float>("moveWaves", move_waves);
            WATER_SHADER.set_vec3("cameraPos", glm::vec3(scene_camera.position));
            WATER.draw(WATER_SHADER);
        WATER_SHADER.off();

        // ImGUI
        gui.draw(scene_camera);
        gui.off();

        // Swap the back- with the front-buffer that is used for screen display
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    //----------------------------------------MAIN LOOP---------------------------------------------------

    glDeleteVertexArrays(1, &SKYBOX_VAO);
    glDeleteBuffers(1, &SKYBOX_TEXT);

    // Cleanup glfw
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
