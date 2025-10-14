#ifndef MATF_RG_PROJECT_INSTANCECONTROLLER_H
#define MATF_RG_PROJECT_INSTANCECONTROLLER_H

#include <engine/core/Controller.hpp>
#include <glm/glm.hpp>

namespace engine::graphics {

class InstanceController : public engine::core::Controller {
public:
    std::string_view name() const override;

    int road_segment_count = 20;
    float road_spacing = 12.0f;
    glm::vec3 road_start_position = glm::vec3(5.0f, -1.0f, -20.0f);
    glm::vec3 road_scale = glm::vec3(0.02f);
    bool use_instanced_rendering = true;

    int lamp_count = 10;
    float lamp_spacing = 8.0f;
    glm::vec3 lamp_start_position = glm::vec3(6.0f, -0.7f, -8.0f);
    glm::vec3 lamp_scale = glm::vec3(0.015f);
    bool use_lamp_instancing = true;

private:
    void initialize() override;
    void draw() override;
    void poll_events() override;
};

} // namespace engine::graphics

#endif //MATF_RG_PROJECT_INSTANCECONTROLLER_H