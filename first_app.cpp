#include "first_app.hpp"
#include <GLFW/glfw3.h>

namespace lve {

FirstApp::FirstApp() : lveWindow(WIDTH, HEIGHT, "window"){}

void FirstApp::run() {
    while(!lveWindow.shouldClose()) {
        glfwPollEvents();
    }
}

}