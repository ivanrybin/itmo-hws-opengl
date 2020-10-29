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

#include "opengl_shader.h"

#include "project/texture.h"
#include "project/center_camera.h"
#include "project/model.h"
#include "project/skybox.h"

float deltaTime = 0.0f;	// время между текущим кадром и последним кадром
float lastFrame = 0.0f;

const unsigned int WIN_WIDTH  = 1280;
const unsigned int WIN_HEIGHT =  720;

bool is_start = true;
double x_prev, y_prev;

camera::camera center_camera{};


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

    center_camera.process_mouse(x_offset, y_offset);
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset) {
    center_camera.process_mouse_scroll(y_offset);
}

void process_input(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void glfw_error_callback(int error, const char *description)  {
    std::cerr << fmt::format("Glfw Error {}: {}\n", error, description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

GLuint load_cube_map() {
    std::vector<std::string> faces = { "right.jpg", "left.jpg",
                                       "top.jpg",   "bottom.jpg",
                                       "front.jpg", "back.jpg" };
    for (int i = 0; i < faces.size(); ++i) {
        faces[i] = "../textures/skybox/" + faces[i];
    }
    return my_model::cube_map_loader(faces);;
}


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
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();

    // VBO, VAO ------------------------------------------------------------------------------------------
    GLuint skyboxVBO, skyboxVAO;
    my_model::skybox SKY_BOX;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(SKY_BOX.coords), &SKY_BOX.coords, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // SHADER --------------------------------------------------------------------------------------------

    shader_t PROGRAM("../assets/reflect_VS.glsl", "../assets/reflect_FS.glsl");
    shader_t SKYBOX_SHADER("../assets/skybox_VS.glsl", "../assets/skybox_FS.glsl");
    my_model::model model1("../textures/backpack/backpack.obj", "../textures/backpack/");
    GLuint cubemapTexture = load_cube_map();

    // ImGUI PARAMETERS ----------------------------------------------------------------------------------
    bool is_polygons = false;   // polygons
    int RC = 1, GC = 0, BC = 0; // colors background

    float n1 = 1.0;
    float n2 = 1.5;
    float color_percent = 0.1;

    PROGRAM.use();
    PROGRAM.set_uniform<int>("skybox", cubemapTexture);

    SKYBOX_SHADER.use();
    SKYBOX_SHADER.set_uniform<int>("skybox", cubemapTexture);

    // Z-TEST
    glEnable(GL_DEPTH_TEST);
    //----------------------------------------MAIN LOOP---------------------------------------------------
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        process_input(window);

        // Fill background with solid color
        glClearColor(RC / 255.0f, GC / 255.0f, BC / 255.0f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ImGUI - BEG - Gui start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Parameters");
        ImGui::Checkbox("polygons", &is_polygons);
        (is_polygons) ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        ImGui::SliderFloat("color percent", &color_percent, 0.0f, 1.0f);
        ImGui::SliderFloat("world refractive idx", &n1, -10.0f, 10.0f);
        ImGui::SliderFloat("model refractive idx", &n2, -10.0f, 10.0f);

        ImGui::End();
        // ImGUI - END

        // PROGRAM ------------------------------
        PROGRAM.use();

        glm::mat4 view = center_camera.get_view_matrix();
        glm::mat4 model = glm::scale(glm::vec3(center_camera.zoom, center_camera.zoom, center_camera.zoom));
        glm::mat4 projection = glm::perspective<float>(45, (float) WIN_WIDTH / (float) WIN_HEIGHT, 0.1f, 100.0f);

        PROGRAM.set_mat4("view", view);
        PROGRAM.set_mat4("model", model);
        PROGRAM.set_mat4("projection", projection);
        PROGRAM.set_vec3("cameraPos", glm::vec3(center_camera.position));

        PROGRAM.set_uniform<float>("color_percent", color_percent);
        PROGRAM.set_uniform<float>("n1", n1);
        PROGRAM.set_uniform<float>("n2", n2);
        PROGRAM.set_uniform<int>("skybox", cubemapTexture);

        // MODEL DRAW---------------------------------------
        model1.draw(PROGRAM);
        // MODEL DRAW---------------------------------------

        // отрисовываем скайбокс последним
        glDepthFunc(GL_LEQUAL);  // меняем функцию глубины, чтобы обеспечить прохождение теста глубины, когда значения равны содержимому буфера глубины

        SKYBOX_SHADER.use();
        SKYBOX_SHADER.set_mat4("view", glm::mat4(glm::mat3(view)));
        SKYBOX_SHADER.set_mat4("projection", projection);
        SKYBOX_SHADER.set_uniform<int>("skybox", cubemapTexture);

        // куб скайбокса
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0 + cubemapTexture);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // восстанавливаем стандартное значение функции теста глубины

        // Generate gui render commands
        ImGui::Render();
        // Execute gui render commands using OpenGL backend
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap the back- with the front-buffer that is used for screen display
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    //----------------------------------------MAIN LOOP---------------------------------------------------

    // Cleanup ImGUI
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVAO);

    // Cleanup glfw
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
