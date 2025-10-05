#ifndef MATF_RG_PROJECT_GUICONTROLLER_H
#define MATF_RG_PROJECT_GUICONTROLLER_H
#include <engine/core/Controller.hpp>
#include <glm/glm.hpp>
#include <array>

namespace app {
class InstanceController;

struct PointLight {
    bool enabled = false;
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 color = glm::vec3(1.0f, 0.9f, 0.7f);
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
};

struct SpotLight {
    bool enabled = true;
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
    glm::vec3 color = glm::vec3(1.0f, 0.9f, 0.7f);
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
    float cutOff = 12.5f;
    float outerCutOff = 15.0f;
};

class GuiController : public engine::core::Controller {
public:
    std::string_view name() const override;

    glm::vec3 sunlight_direction = glm::vec3(1.0f, -0.2f, -0.2f);
    glm::vec3 sunlight_ambient = glm::vec3(0.4f, 0.35f, 0.2f);
    glm::vec3 sunlight_diffuse = glm::vec3(1.2f, 1.0f, 0.8f);
    glm::vec3 sunlight_specular = glm::vec3(1.0f, 0.95f, 0.9f);

    std::array<PointLight, 3> point_lights;
    std::array<SpotLight, 2> spot_lights;

private:
    void initialize() override;
    void draw() override;
    void poll_events() override;
};

}// namespace app

#endif//MATF_RG_PROJECT_GUICONTROLLER_H
