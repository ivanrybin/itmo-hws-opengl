#include "gui_handler.h"
#include "imgui.h"
#include "../../bindings/imgui_impl_glfw.h"
#include "../../bindings/imgui_impl_opengl3.h"

gui_handler::gui_handler(GLFWwindow* window, const char *glsl_version) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();
}

gui_handler::~gui_handler() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void gui_handler::draw(camera::camera& scene_camera) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Parameters");

    // ImGui::Checkbox("polygons", &is_polygons);
    ImGui::Checkbox("skybox", &is_skybox);

    (is_polygons) ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    ImGui::SliderFloat("camera speed ", &scene_camera.move_speed, 0.0f, 1000.0f);
    ImGui::SliderInt("tiles number ", &texture_repeats, 0, 30);

    ImGui::SliderFloat("ambient strength", &ambient_strength, 0.0f, 1.0f);
    // ImGui::SliderFloat("diffuse strength", &diffuse_strength, 0.0f, 1.0f);

    // ImGui::SliderFloat("light LX", &LX, -200.0f, 200.0f);
    // ImGui::SliderFloat("light LY", &LY, -200.0f, 200.0f);
    // ImGui::SliderFloat("light LZ", &LZ, -200.0f, 200.0f);

    ImGui::SliderFloat("R", &LR, 0.0f, 1.0f);
    ImGui::SliderFloat("G", &LG, 0.0f, 1.0f);
    ImGui::SliderFloat("B", &LB, 0.0f, 1.0f);

    // ImGui::SliderFloat("clip dist", &clip_dist, -1.0f, 1.0f);

    ImGui::End();
}

void gui_handler::off() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


GLuint init_square(float* s) {
    unsigned int indices[6] = {
            0, 1, 2,
            3, 2, 1
    };

    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 20, s, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    return VAO;
}

void draw_square(GLuint VAO) {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}