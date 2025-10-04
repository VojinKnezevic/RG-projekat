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
    engine::resources::Model *road = resource->model("road_segment");

    engine::resources::Shader *shader = resource->shader("road");

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    for (int i = 0; i < 3; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(5.0f, -1.0f, 10.0f * i - 3.0f));
        model = glm::scale(model, glm::vec3(0.02f));

        shader->set_mat4("model", model);
        road->draw(shader);
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
