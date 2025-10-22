#include "MyApp.hpp"


#include <engine/core/Engine.hpp>

#include <iostream>


/**
 * Start here...
 */
int main(int argc, char** argv) {

    std::cout << "Hello World!" << std::endl;

    auto app = std::make_unique<app::MyApp>();
    return app->run(argc, argv);

    return 0;
}
