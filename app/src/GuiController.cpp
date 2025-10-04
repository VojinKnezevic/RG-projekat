#include "../include/GuiController.hpp"

#include "engine/graphics/GraphicsController.hpp"
#include "engine/platform/PlatformController.hpp"
#include "imgui.h"

namespace app {
std::string_view GuiController::name() const {
    return "app::GuiController";
}
void GuiController::initialize() {
    set_enable(false);
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->set_enable_cursor(false);
}
void GuiController::draw() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    graphics->begin_gui();

    auto camera = graphics->camera();


    ImGui::Begin("Settings");

    ImGui::Text("Camera pos(%f %f %f)", camera->Position.x, camera->Position.y, camera->Position.z);

    ImGui::End();

    graphics->end_gui();
}
void GuiController::poll_events() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_F2).state() == engine::platform::Key::State::JustPressed) {
        set_enable(!is_enabled());
        platform->set_enable_cursor(is_enabled());
    }
}
}// namespace app
