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
    void draw() override;

    void update_camera();
    void update() override;

public:
    std::string_view name() const override {
        return "app::MainController";
    }
};

}// namespace app

#endif//MATF_RG_PROJECT_MAINCONTROLLER_H
