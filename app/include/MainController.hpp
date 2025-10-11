#ifndef MATF_RG_PROJECT_MAINCONTROLLER_H
#define MATF_RG_PROJECT_MAINCONTROLLER_H
#include "engine/core/Controller.hpp"

namespace app {

class MainController : public engine::core::Controller{
    void initialize() override;

bool loop() override;
    void begin_draw() override;
    void end_draw() override;
    void draw_backpack();
    void draw_road();
    void draw_lamp_post();
    void draw_light_bulbs();
    void draw_nissan();
    void draw_skybox();
    void draw_test_cube();
    void draw() override;

    void update_camera();
    void update() override;
    void setup_cube();

    unsigned int m_cube_vao = 0;
    unsigned int m_cube_vbo = 0;

public:
    std::string_view name() const override {
        return "app::MainController";
    }
};

}// namespace app

#endif//MATF_RG_PROJECT_MAINCONTROLLER_H
