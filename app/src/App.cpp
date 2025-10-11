#include "GuiController.hpp"
#include "MainController.hpp"
#include "InstanceController.hpp"
#include "engine/graphics/BloomController.hpp"
#include "spdlog/spdlog.h"


#include <MyApp.hpp>

namespace app {
void MyApp::app_setup() {
    spdlog::info("App setup completed");

    auto main_controller = register_controller<app::MainController>();
    auto gui_controller = register_controller<app::GuiController>();
    auto instance_controller = register_controller<app::InstanceController>();
    auto bloom_controller = register_controller<engine::graphics::BloomController>();

    main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
    main_controller->before(gui_controller);
    instance_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
    instance_controller->before(main_controller);
    bloom_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
    bloom_controller->before(main_controller);
}
}
