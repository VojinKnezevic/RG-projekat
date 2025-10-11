#include "../include/MainController.hpp"

#include "GuiController.hpp"
#include "InstanceController.hpp"
#include "engine/graphics/BloomController.hpp"
#include "engine/graphics/GraphicsController.hpp"
#include "engine/graphics/OpenGL.hpp"
#include "engine/platform/PlatformController.hpp"
#include "engine/resources/Mesh.hpp"
#include "engine/resources/ResourcesController.hpp"
#include <glad/glad.h>
#include "spdlog/spdlog.h"

namespace app {

class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
public:
    void on_mouse_move(engine::platform::MousePosition position) override;
};

void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    auto gui_controller = engine::core::Controller::get<GuiController>();
    if (!gui_controller->is_enabled())
        camera->rotate_camera(position.dx, position.dy);
}


void MainController::initialize() {
    auto platform = engine::platform::PlatformController::get<engine::platform::PlatformController>();
    platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());
    engine::graphics::OpenGL::enable_depth_testing();
    setup_cube();
    spdlog::info("Main controller initialized!!");
}
bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KEY_ESCAPE).is_down()) {
        return false;
    }

    return true;
}
void MainController::begin_draw() {
    auto bloom_controller = engine::core::Controller::get<engine::graphics::BloomController>();
    bloom_controller->prepare_hdr();
}
void MainController::end_draw() {
    auto bloom_controller = engine::core::Controller::get<engine::graphics::BloomController>();
    bloom_controller->finalize_bloom();

    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}
void MainController::draw_road() {
    auto resource = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto gui_controller = engine::core::Controller::get<GuiController>();
    auto instance_controller = engine::core::Controller::get<InstanceController>();
    engine::resources::Model *road = resource->model("road_segment");

    engine::resources::Shader *shader = resource->shader("basic");

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_vec3("viewPos", graphics->camera()->Position);

    shader->set_vec3("dirLight.direction", gui_controller->sunlight_direction);
    shader->set_vec3("dirLight.ambient", gui_controller->sunlight_ambient);
    shader->set_vec3("dirLight.diffuse", gui_controller->sunlight_diffuse);
    shader->set_vec3("dirLight.specular", gui_controller->sunlight_specular);

    shader->set_int("material.diffuse", 0);
    shader->set_int("material.specular", 1);
    shader->set_float("material.shininess", 64.0f);

    for (int i = 0; i < 4; i++) {
        std::string base = "pointLights[" + std::to_string(i) + "]";
        shader->set_bool(base + ".enabled", gui_controller->point_lights[i].enabled);
        shader->set_vec3(base + ".position", gui_controller->point_lights[i].position);
        shader->set_vec3(base + ".color", gui_controller->point_lights[i].color);
        shader->set_float(base + ".constant", gui_controller->point_lights[i].constant);
        shader->set_float(base + ".linear", gui_controller->point_lights[i].linear);
        shader->set_float(base + ".quadratic", gui_controller->point_lights[i].quadratic);
    }

    for (int i = 0; i < 2; i++) {
        std::string base = "spotLights[" + std::to_string(i) + "]";
        shader->set_bool(base + ".enabled", gui_controller->spot_lights[i].enabled);
        shader->set_vec3(base + ".position", gui_controller->spot_lights[i].position);
        shader->set_vec3(base + ".direction", gui_controller->spot_lights[i].direction);
        shader->set_vec3(base + ".color", gui_controller->spot_lights[i].color);
        shader->set_float(base + ".constant", gui_controller->spot_lights[i].constant);
        shader->set_float(base + ".linear", gui_controller->spot_lights[i].linear);
        shader->set_float(base + ".quadratic", gui_controller->spot_lights[i].quadratic);
        shader->set_float(base + ".cutOff", gui_controller->spot_lights[i].cutOff);
        shader->set_float(base + ".outerCutOff", gui_controller->spot_lights[i].outerCutOff);
    }

    // Simple road rendering - check if we should use instanced count
    int roadCount = 3; // default
    glm::vec3 startPos = glm::vec3(5.0f, -1.0f, -3.0f);
    float spacing = 12.0f;
    glm::vec3 scale = glm::vec3(0.02f);

    if (instance_controller->use_instanced_rendering) {
        roadCount = instance_controller->road_segment_count;
        startPos = instance_controller->road_start_position;
        spacing = instance_controller->road_spacing;
        scale = instance_controller->road_scale;
    }

    for (int i = 0; i < roadCount; i++) {
        glm::mat4 model = glm::mat4(1.0f);

        // Center the road layout: offset so the middle road is at startPos
        float offset = spacing * (i - roadCount / 2.0f);
        glm::vec3 position = startPos + glm::vec3(0, 0, offset);

        model = glm::translate(model, position);
        model = glm::scale(model, scale);

        shader->set_mat4("model", model);
        road->draw(shader);
    }
}
void MainController::draw_lamp_post() {
    auto resource = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto gui_controller = engine::core::Controller::get<GuiController>();
    engine::resources::Model *lamp_post = resource->model("lamp_post");

    engine::resources::Shader *shader = resource->shader("basic");

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_vec3("viewPos", graphics->camera()->Position);

    shader->set_vec3("dirLight.direction", gui_controller->sunlight_direction);
    shader->set_vec3("dirLight.ambient", gui_controller->sunlight_ambient);
    shader->set_vec3("dirLight.diffuse", gui_controller->sunlight_diffuse);
    shader->set_vec3("dirLight.specular", gui_controller->sunlight_specular);

    shader->set_int("material.diffuse", 0);
    shader->set_int("material.specular", 1);
    shader->set_float("material.shininess", 32.0f);

    for (int i = 0; i < 4; i++) {
        std::string base = "pointLights[" + std::to_string(i) + "]";
        shader->set_bool(base + ".enabled", gui_controller->point_lights[i].enabled);
        shader->set_vec3(base + ".position", gui_controller->point_lights[i].position);
        shader->set_vec3(base + ".color", gui_controller->point_lights[i].color);
        shader->set_float(base + ".constant", gui_controller->point_lights[i].constant);
        shader->set_float(base + ".linear", gui_controller->point_lights[i].linear);
        shader->set_float(base + ".quadratic", gui_controller->point_lights[i].quadratic);
    }

    for (int i = 0; i < 2; i++) {
        std::string base = "spotLights[" + std::to_string(i) + "]";
        shader->set_bool(base + ".enabled", gui_controller->spot_lights[i].enabled);
        shader->set_vec3(base + ".position", gui_controller->spot_lights[i].position);
        shader->set_vec3(base + ".direction", gui_controller->spot_lights[i].direction);
        shader->set_vec3(base + ".color", gui_controller->spot_lights[i].color);
        shader->set_float(base + ".constant", gui_controller->spot_lights[i].constant);
        shader->set_float(base + ".linear", gui_controller->spot_lights[i].linear);
        shader->set_float(base + ".quadratic", gui_controller->spot_lights[i].quadratic);
        shader->set_float(base + ".cutOff", gui_controller->spot_lights[i].cutOff);
        shader->set_float(base + ".outerCutOff", gui_controller->spot_lights[i].outerCutOff);
    }

    // Simple lamp rendering - check if we should use instanced count
    auto instance_controller = engine::core::Controller::get<InstanceController>();
    int lampCount = 3; // default
    glm::vec3 startPos = glm::vec3(6.0f, -0.7f, -8.0f);
    float spacing = 8.0f;
    glm::vec3 scale = glm::vec3(0.015f);

    if (instance_controller->use_lamp_instancing) {
        lampCount = instance_controller->lamp_count;
        startPos = instance_controller->lamp_start_position;
        spacing = instance_controller->lamp_spacing;
        scale = instance_controller->lamp_scale;
    }

    for (int i = 0; i < lampCount; i++) {
        glm::mat4 model = glm::mat4(1.0f);

        // Always start from startPos and add lamps in sequence
        glm::vec3 position = startPos + glm::vec3(0, 0, spacing * i);

        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, scale);
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        shader->set_mat4("model", model);
        lamp_post->draw(shader);
    }
}
void MainController::draw_light_bulbs() {
    auto resource = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto gui_controller = engine::core::Controller::get<GuiController>();
    engine::resources::Model *light_bulb = resource->model("light_bulb");

    engine::resources::Shader *shader = resource->shader("basic");

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_vec3("viewPos", graphics->camera()->Position);

    shader->set_vec3("dirLight.direction", gui_controller->sunlight_direction);
    shader->set_vec3("dirLight.ambient", gui_controller->sunlight_ambient);
    shader->set_vec3("dirLight.diffuse", gui_controller->sunlight_diffuse);
    shader->set_vec3("dirLight.specular", gui_controller->sunlight_specular);

    shader->set_int("material.diffuse", 0);
    shader->set_int("material.specular", 1);
    shader->set_float("material.shininess", 32.0f);

    for (int i = 0; i < 3; i++) {
        std::string base = "pointLights[" + std::to_string(i) + "]";
        shader->set_bool(base + ".enabled", gui_controller->point_lights[i].enabled);
        shader->set_vec3(base + ".position", gui_controller->point_lights[i].position);

        shader->set_vec3(base + ".color", gui_controller->point_lights[i].color);
        shader->set_float(base + ".constant", gui_controller->point_lights[i].constant);
        shader->set_float(base + ".linear", gui_controller->point_lights[i].linear);
        shader->set_float(base + ".quadratic", gui_controller->point_lights[i].quadratic);
    }

    for (int i = 0; i < 2; i++) {
        std::string base = "spotLights[" + std::to_string(i) + "]";
        shader->set_bool(base + ".enabled", gui_controller->spot_lights[i].enabled);
        shader->set_vec3(base + ".position", gui_controller->spot_lights[i].position);
        shader->set_vec3(base + ".direction", gui_controller->spot_lights[i].direction);
        shader->set_vec3(base + ".color", gui_controller->spot_lights[i].color);
        shader->set_float(base + ".constant", gui_controller->spot_lights[i].constant);
        shader->set_float(base + ".linear", gui_controller->spot_lights[i].linear);
        shader->set_float(base + ".quadratic", gui_controller->spot_lights[i].quadratic);
        shader->set_float(base + ".cutOff", gui_controller->spot_lights[i].cutOff);
        shader->set_float(base + ".outerCutOff", gui_controller->spot_lights[i].outerCutOff);
    }

    for (int i = 0; i < 3; i++) {
        if (gui_controller->point_lights[i].enabled) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, gui_controller->point_lights[i].position);
            model = glm::scale(model, glm::vec3(0.8f));
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

            shader->set_mat4("model", model);
            light_bulb->draw(shader);
        }
    }
}
void MainController::draw_nissan() {
    auto resource = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto gui_controller = engine::core::Controller::get<GuiController>();
    engine::resources::Model *nissan = resource->model("nissan");

    engine::resources::Shader *shader = resource->shader("obj");

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_vec3("viewPos", graphics->camera()->Position);

    shader->set_vec3("dirLight.direction", gui_controller->sunlight_direction);
    shader->set_vec3("dirLight.ambient", gui_controller->sunlight_ambient);
    shader->set_vec3("dirLight.diffuse", gui_controller->sunlight_diffuse);
    shader->set_vec3("dirLight.specular", gui_controller->sunlight_specular);

    shader->set_int("material.diffuse", 0);
    shader->set_int("material.specular", 1);
    shader->set_float("material.shininess", 32.0f);

    for (int i = 0; i < 4; i++) {
        std::string base = "pointLights[" + std::to_string(i) + "]";
        shader->set_bool(base + ".enabled", gui_controller->point_lights[i].enabled);
        shader->set_vec3(base + ".position", gui_controller->point_lights[i].position);
        shader->set_vec3(base + ".color", gui_controller->point_lights[i].color);
        shader->set_float(base + ".constant", gui_controller->point_lights[i].constant);
        shader->set_float(base + ".linear", gui_controller->point_lights[i].linear);
        shader->set_float(base + ".quadratic", gui_controller->point_lights[i].quadratic);
    }

    for (int i = 0; i < 2; i++) {
        std::string base = "spotLights[" + std::to_string(i) + "]";
        shader->set_bool(base + ".enabled", gui_controller->spot_lights[i].enabled);
        shader->set_vec3(base + ".position", gui_controller->spot_lights[i].position);
        shader->set_vec3(base + ".direction", gui_controller->spot_lights[i].direction);
        shader->set_vec3(base + ".color", gui_controller->spot_lights[i].color);
        shader->set_float(base + ".constant", gui_controller->spot_lights[i].constant);
        shader->set_float(base + ".linear", gui_controller->spot_lights[i].linear);
        shader->set_float(base + ".quadratic", gui_controller->spot_lights[i].quadratic);
        shader->set_float(base + ".cutOff", gui_controller->spot_lights[i].cutOff);
        shader->set_float(base + ".outerCutOff", gui_controller->spot_lights[i].outerCutOff);
    }

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(12.0f, -0.1f, 3.0f));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(2.0f));


    shader->set_mat4("model", model);
    nissan->draw(shader);
}
void MainController::draw_skybox() {
    auto resource = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto skybox = resource->skybox("sunset_sb");

    auto shader = resource->shader("skybox");
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    graphics->draw_skybox(shader,skybox);
}




void MainController::draw() {
    draw_road();
    draw_lamp_post();
    draw_light_bulbs();
    draw_nissan();
    draw_skybox();
    draw_test_cube();
}
void MainController::update_camera() {
    auto gui_controller = engine::core::Controller::get<GuiController>();
    if (gui_controller->is_enabled()) {
        return;
    }
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    auto camera = graphics->camera();
    float dt = platform->dt();
    if (platform->key(engine::platform::KEY_W).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt);
    }
    if (platform->key(engine::platform::KEY_S).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt);
    }
    if (platform->key(engine::platform::KEY_A).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt);
    }
    if (platform->key(engine::platform::KEY_D).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt);
    }
    if (platform->key(engine::platform::KEY_LEFT_SHIFT).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::DOWN, dt);
    }
    if (platform->key(engine::platform::KEY_SPACE).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::UP, dt);
    }
}
void MainController::update() {
    update_camera();
}

void MainController::setup_cube() {
    float cube_vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    glGenVertexArrays(1, &m_cube_vao);
    glGenBuffers(1, &m_cube_vbo);

    glBindVertexArray(m_cube_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_cube_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void MainController::draw_test_cube() {
    auto resource = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto gui_controller = engine::core::Controller::get<GuiController>();

    auto shader = resource->shader("uniform_color");
    if (!shader) {
        return;
    }

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, gui_controller->test_cube_position);
    model = glm::scale(model, gui_controller->test_cube_scale);

    shader->set_mat4("model", model);
    shader->set_vec3("color", gui_controller->test_cube_color);

    glBindVertexArray(m_cube_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

}// namespace app
