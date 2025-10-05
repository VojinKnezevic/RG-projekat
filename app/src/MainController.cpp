#include "../include/MainController.hpp"

#include "GuiController.hpp"
#include "engine/graphics/GraphicsController.hpp"
#include "engine/graphics/OpenGL.hpp"
#include "engine/platform/PlatformController.hpp"
#include "engine/resources/Mesh.hpp"
#include "engine/resources/ResourcesController.hpp"
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
    engine::graphics::OpenGL::clear_buffers();
}
void MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}
void MainController::draw_road() {
    auto resource = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto gui_controller = engine::core::Controller::get<GuiController>();
    engine::resources::Model *road = resource->model("road_segment");

    engine::resources::Shader *shader = resource->shader("gltf");

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

    for (int i = 0; i < 3; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(5.0f, -1.0f, 12.0f * i - 3.0f));
        model = glm::scale(model, glm::vec3(0.02f));

        shader->set_mat4("model", model);
        road->draw(shader);
    }
}
void MainController::draw_lamp_post() {
    auto resource = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto gui_controller = engine::core::Controller::get<GuiController>();
    engine::resources::Model *lamp_post = resource->model("lamp_post");

    engine::resources::Shader *shader = resource->shader("gltf");

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

    for (int i = 0; i < 3; i++) {
        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, glm::vec3(6.0f, -0.7f, -8.0f + 8.0f * i));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        model = glm::scale(model, glm::vec3(0.015f));

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

    engine::resources::Shader *shader = resource->shader("gltf");

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
void MainController::draw_skybox() {
    auto resource = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto skybox = resource->skybox("mt_skybox");

    auto shader = resource->shader("skybox");
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    graphics->draw_skybox(shader,skybox);
}




void MainController::draw() {
    draw_road();
    draw_lamp_post();
    draw_light_bulbs();
    draw_skybox();
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

}// namespace app
