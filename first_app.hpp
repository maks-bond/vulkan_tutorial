#pragma once

#include "lve_window.hpp"

namespace lve {

class FirstApp {
public:
    FirstApp();

    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    void run();

private:
    LveWindow lveWindow;

};

}