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

    for (int i = 0; i < 3; i++) {
        point_lights[i].enabled = true;
        point_lights[i].position = glm::vec3(6.71111f, 1.95f, -8.0f + 8.0f * i);
        point_lights[i].color = glm::vec3(1.0f, 0.9f, 0.7f);
        point_lights[i].constant = 1.0f;
        point_lights[i].linear = 0.09f;
        point_lights[i].quadratic = 0.032f;
    }

    // Initialize car headlight spotlights
    // Headlight 1 (Left)
    spot_lights[0].enabled = true;
    spot_lights[0].position = glm::vec3(11.364f, 0.227f, 5.843f);
    spot_lights[0].direction = glm::vec3(0.227f, -0.159f, -1.0f);
    spot_lights[0].color = glm::vec3(1.0f, 0.9f, 0.7f); // Warm white
    spot_lights[0].constant = 0.169f;
    spot_lights[0].linear = 0.001f;
    spot_lights[0].quadratic = 0.001f;
    spot_lights[0].cutOff = 7.083f;
    spot_lights[0].outerCutOff = 9.654f;

    // Headlight 2 (Right)
    spot_lights[1].enabled = true;
    spot_lights[1].position = glm::vec3(12.464f, 0.227f, 5.843f);
    spot_lights[1].direction = glm::vec3(-0.227f, -0.159f, -1.0f);
    spot_lights[1].color = glm::vec3(1.0f, 0.9f, 0.7f); // Warm white
    spot_lights[1].constant = 0.169f;
    spot_lights[1].linear = 0.001f;
    spot_lights[1].quadratic = 0.001f;
    spot_lights[1].cutOff = 7.083f;
    spot_lights[1].outerCutOff = 9.654f;
}
void GuiController::draw() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    graphics->begin_gui();

    auto camera = graphics->camera();


    ImGui::Begin("Settings");

    ImGui::Text("Camera pos(%f %f %f)", camera->Position.x, camera->Position.y, camera->Position.z);

    ImGui::Separator();

    if (ImGui::CollapsingHeader("SUNCE")) {
        ImGui::Text("vec3");
        ImGui::SliderFloat3("##Direction", &sunlight_direction.x, -1.0f, 1.0f);

        ImGui::Separator();

        ImGui::Text("Amb");
        ImGui::ColorEdit3("##Ambient", &sunlight_ambient.x);

        ImGui::Text("Diff");
        ImGui::ColorEdit3("##Diffuse", &sunlight_diffuse.x);

        ImGui::Text("Spec");
        ImGui::ColorEdit3("##Specular", &sunlight_specular.x);
    }

    ImGui::Separator();

    if (ImGui::CollapsingHeader("SPOTLIGHTS")) {
        for (int i = 0; i < 2; i++) {
            ImGui::PushID(i + 100); // Offset ID to avoid conflicts

            std::string header = "Headlight " + std::to_string(i + 1);
            if (ImGui::TreeNode(header.c_str())) {
                ImGui::Checkbox("Enabled", &spot_lights[i].enabled);

                if (spot_lights[i].enabled) {
                    ImGui::Text("Position");
                    ImGui::SliderFloat3("##SpotPosition", &spot_lights[i].position.x, 0.0f, 20.0f);

                    ImGui::Text("Direction");
                    ImGui::SliderFloat3("##SpotDirection", &spot_lights[i].direction.x, -1.0f, 1.0f);

                    ImGui::Text("Color");
                    ImGui::ColorEdit3("##SpotColor", &spot_lights[i].color.x);

                    ImGui::Text("Attenuation");
                    ImGui::SliderFloat("Constant##Spot", &spot_lights[i].constant, 0.1f, 2.0f);
                    ImGui::SliderFloat("Linear##Spot", &spot_lights[i].linear, 0.001f, 1.0f);
                    ImGui::SliderFloat("Quadratic##Spot", &spot_lights[i].quadratic, 0.001f, 1.0f);

                    ImGui::Text("Spotlight Cone");
                    ImGui::SliderFloat("Inner Cutoff", &spot_lights[i].cutOff, 1.0f, 45.0f);
                    ImGui::SliderFloat("Outer Cutoff", &spot_lights[i].outerCutOff, spot_lights[i].cutOff, 50.0f);
                }

                ImGui::TreePop();
            }

            ImGui::PopID();
        }
    }

    ImGui::Separator();

    if (ImGui::CollapsingHeader("POINT LIGHTS")) {
        for (int i = 0; i < 3; i++) {
            ImGui::PushID(i);

            std::string header = "Light " + std::to_string(i + 1);
            if (ImGui::TreeNode(header.c_str())) {
                ImGui::Checkbox("Enabled", &point_lights[i].enabled);

                if (point_lights[i].enabled) {
                    ImGui::Text("Position");
                    ImGui::SliderFloat3("##Position", &point_lights[i].position.x, -20.0f, 20.0f);

                    ImGui::Text("Color");
                    ImGui::ColorEdit3("##Color", &point_lights[i].color.x);

                    ImGui::Text("Attenuation");
                    ImGui::SliderFloat("Constant", &point_lights[i].constant, 0.1f, 2.0f);
                    ImGui::SliderFloat("Linear", &point_lights[i].linear, 0.001f, 1.0f);
                    ImGui::SliderFloat("Quadratic", &point_lights[i].quadratic, 0.001f, 1.0f);
                }

                ImGui::TreePop();
            }

            ImGui::PopID();
        }
    }

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
