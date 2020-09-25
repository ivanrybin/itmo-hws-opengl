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

static void glfw_error_callback(int error, const char *description)  {
    std::cerr << fmt::format("Glfw Error {}: {}\n", error, description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void init_rectangle(GLuint& VBO, GLuint& VAO, GLuint& EBO) {
    // create the rectangle
    constexpr float vertices[] {
            -1.0f, -1.0f, 0.0f, //  0       3 ------ 2
             1.0f, -1.0f, 0.0f, //  1       |        |
             1.0f,  1.0f, 0.0f, //  2       |        |
            -1.0f,  1.0f, 0.0f, //  3       0 ------ 1
    };
//    constexpr float vertices[] {
//            -0.5f, -0.5f, 0.0f, //  0       3 ------ 2
//             0.5f, -0.5f, 0.0f, //  1       |        |
//             0.5f,  0.5f, 0.0f, //  2       |        |
//            -0.5f,  0.5f, 0.0f, //  3       0 ------ 1
//    };

    unsigned int indices[6] = {
            0, 1, 2, // первый
            0, 2, 3  // второй
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void process_input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

const unsigned int WIN_WIDTH  = 1280;
const unsigned int WIN_HEIGHT =  720;

bool is_start = true;
double x_prev, y_prev;
double x_last, y_last;

struct mandelbrot {
    void change_coords(float x_offset, float y_offset) {
        x_left -= (x_offset / WIN_WIDTH) * (x_right - x_left);
        x_right -= (x_offset / WIN_WIDTH) * (x_right - x_left);

        y_down -= (y_offset / WIN_HEIGHT) * (y_up - y_down);
        y_up -= (y_offset / WIN_HEIGHT) * (y_up - y_down);

    }
    void change_scale(float sign) {
        float x1_left = (x_last / WIN_WIDTH) * (x_right - x_left);
        float x1_right = (WIN_WIDTH - x_last) / WIN_WIDTH * (x_right - x_left);
        float y1_up = (y_last / WIN_HEIGHT) * (y_up - y_down);
        float y1_down = (WIN_HEIGHT - y_last) / WIN_HEIGHT * (y_up - y_down);

        if (sign > 0) {
            x_left = (x_left + x1_left * 0.1);
            x_right = (x_right - x1_right * 0.1);
            y_up = (y_up - y1_up * 0.1);
            y_down = (y_down + y1_down * 0.1);
        } else {
            x_left = (x_left - x1_left / 9);
            x_right = (x_right + x1_right / 9);
            y_up = (y_up + y1_up / 9);
            y_down = (y_down - y1_down / 9);
        }

    }
    float scale;
    float x, y;
    float x_left, x_right;
    float y_down, y_up;
    unsigned int iters_cnt;
};

// create mandelbrot fractal
mandelbrot m_data{1.0f, 0.0f, 0.0f, -2.0f, 1.0f, -1.0f, 1.0f, 40};

void mouse_callback(GLFWwindow* window, double x_curr, double y_curr) {
    // пропуск мыши на окне imgui
    if (ImGui::IsAnyWindowHovered() || ImGui::IsAnyItemHovered() || ImGui::IsAnyItemActive()) {
        return;
    }

    x_last = x_curr;
    y_last = y_curr;

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

    m_data.change_coords(x_offset, y_offset);
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset) {
    m_data.change_scale(y_offset);
}


int main(int, char **) {
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
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

    // Create window with graphics context
    GLFWwindow *window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "OpenGL Mandelbrot fractal -- Rybin Ivan, ITMO JB SE", NULL, NULL);
    if (window == NULL) {
       return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize GLEW, i.e. fill all possible function pointers for current OpenGL context
    if (glewInit() != GLEW_OK) {
      std::cerr << "Failed to initialize OpenGL loader!\n";
      return 1;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // MOUSE
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // init buffer and geometries
    GLuint VBO, VAO, EBO;
    init_rectangle(VBO, VAO, EBO);

    // init shader
    shader_t program_1("simple_VS.glsl", "fractal_FS.glsl");


    // Setup GUI context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();

    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        // Fill background with solid color
        glClearColor(0.1f, 0.1f, 0.1f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Gui start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ImGUI
        ImGui::Begin("Mandelbrot parameters");
        static int iterations = 40;
        ImGui::SliderInt("iterations", &iterations, 1, 2000);
        static int color_offset = 0;
        ImGui::SliderInt("color offset", &color_offset, 50, 100);
        ImGui::End();

        m_data.iters_cnt = iterations;

        // направляем данные в Fragment SHADER
        program_1.set_vec2("rect_size", glm::vec2(WIN_WIDTH, WIN_HEIGHT));
        program_1.set_vec2("area_w",  glm::vec2(m_data.x_left + m_data.x,  m_data.x_right + m_data.x));
        program_1.set_vec2("area_h",  glm::vec2(m_data.y_down + m_data.y,     m_data.y_up + m_data.y));
        program_1.set_uniform("iters_cnt", m_data.iters_cnt);
        program_1.set_uniform("color_offset", color_offset);

        // рисуем
        program_1.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);

        // Generate gui render commands
        ImGui::Render();
        // Execute gui render commands using OpenGL backend
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        // Swap the backbuffer with the frontbuffer that is used for screen display
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
