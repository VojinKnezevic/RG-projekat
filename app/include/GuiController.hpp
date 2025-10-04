#ifndef MATF_RG_PROJECT_GUICONTROLLER_H
#define MATF_RG_PROJECT_GUICONTROLLER_H
#include <engine/core/Controller.hpp>
#include <glm/glm.hpp>
namespace app {

class GuiController : public engine::core::Controller {
public:
    std::string_view name() const override;

    glm::vec3 sunlight_direction = glm::vec3(0.3f, -0.7f, 0.2f);
    glm::vec3 sunlight_ambient = glm::vec3(0.4f, 0.35f, 0.2f);
    glm::vec3 sunlight_diffuse = glm::vec3(1.2f, 1.0f, 0.8f);
    glm::vec3 sunlight_specular = glm::vec3(1.0f, 0.95f, 0.9f);

private:
    void initialize() override;
    void draw() override;
    void poll_events() override;
};

}// namespace app

#endif//MATF_RG_PROJECT_GUICONTROLLER_H
