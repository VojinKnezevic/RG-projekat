#include "../include/GuiController.hpp"
#include "../include/InstanceController.hpp"

#include "engine/graphics/GraphicsController.hpp"
#include "engine/graphics/BloomController.hpp"
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
        point_lights[i].color = glm::vec3(5.0f, 4.0f, 2.0f);
        point_lights[i].constant = 1.0f;
        point_lights[i].linear = 0.09f;
        point_lights[i].quadratic = 0.032f;
    }

    // Initialize car headlight spotlights
    // Headlight 1 (Left)
    spot_lights[0].enabled = true;
    spot_lights[0].position = glm::vec3(11.364f, 0.227f, 5.843f);
    spot_lights[0].direction = glm::vec3(0.227f, -0.159f, -1.0f);
    spot_lights[0].color = glm::vec3(4.0f, 3.5f, 2.5f); // Bright warm white
    spot_lights[0].constant = 0.169f;
    spot_lights[0].linear = 0.001f;
    spot_lights[0].quadratic = 0.001f;
    spot_lights[0].cutOff = 7.083f;
    spot_lights[0].outerCutOff = 9.654f;

    // Headlight 2 (Right)
    spot_lights[1].enabled = true;
    spot_lights[1].position = glm::vec3(12.464f, 0.227f, 5.843f);
    spot_lights[1].direction = glm::vec3(-0.227f, -0.159f, -1.0f);
    spot_lights[1].color = glm::vec3(4.0f, 3.5f, 2.5f); // Bright warm white
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

    ImGui::Separator();

    if (ImGui::CollapsingHeader("BLOOM EFFECT")) {
        auto bloom_controller = engine::core::Controller::get<engine::graphics::BloomController>();

        ImGui::Checkbox("Enable Bloom", &bloom_controller->bloom);
        ImGui::SliderFloat("Exposure", &bloom_controller->exposure, 0.1f, 5.0f);
        ImGui::SliderFloat("Bloom Strength", &bloom_controller->bloom_strength, 0.0f, 2.0f);
        ImGui::SliderInt("Blur Passes", &bloom_controller->bloom_passes, 1, 20);
    }

    ImGui::Separator();

    if (ImGui::CollapsingHeader("TEST CUBE")) {
        ImGui::Text("Position");
        ImGui::SliderFloat3("##CubePosition", &test_cube_position.x, -20.0f, 20.0f);

        ImGui::Text("Scale");
        ImGui::SliderFloat3("##CubeScale", &test_cube_scale.x, 0.1f, 5.0f);

        ImGui::Text("Color (Brightness)");
        ImGui::SliderFloat3("##CubeColor", &test_cube_color.x, 0.0f, 15.0f);
    }

    ImGui::Separator();

    if (ImGui::CollapsingHeader("INSTANCING")) {
        auto instance_controller = engine::core::Controller::get<InstanceController>();

        ImGui::Checkbox("Use Instanced Rendering", &instance_controller->use_instanced_rendering);

        if (ImGui::TreeNode("Road Segments")) {
            ImGui::SliderInt("Count", &instance_controller->road_segment_count, 1, 100);
            ImGui::SliderFloat("Spacing", &instance_controller->road_spacing, 5.0f, 20.0f);

            ImGui::Text("Start Position");
            ImGui::SliderFloat3("##RoadStart", &instance_controller->road_start_position.x, -50.0f, 50.0f);

            ImGui::Text("Scale");
            ImGui::SliderFloat3("##RoadScale", &instance_controller->road_scale.x, 0.01f, 0.1f);

            ImGui::Text("Current count: %d", instance_controller->road_segment_count);

            ImGui::TreePop();
        }

        ImGui::Separator();
        ImGui::Checkbox("Use Lamp Instancing", &instance_controller->use_lamp_instancing);

        if (ImGui::TreeNode("Lamp Posts")) {
            ImGui::SliderInt("Count", &instance_controller->lamp_count, 1, 20);
            ImGui::SliderFloat("Spacing", &instance_controller->lamp_spacing, 2.0f, 15.0f);

            ImGui::Text("Start Position");
            ImGui::SliderFloat3("##LampStart", &instance_controller->lamp_start_position.x, -50.0f, 50.0f);

            ImGui::Text("Scale");
            ImGui::SliderFloat3("##LampScale", &instance_controller->lamp_scale.x, 0.005f, 0.05f);

            ImGui::Text("Current count: %d", instance_controller->lamp_count);

            ImGui::TreePop();
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
