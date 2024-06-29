#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace lve {
class LveWindow {
public:
    LveWindow(int w, int h, const std::string &name);
    ~LveWindow();

    bool shouldClose() {return glfwWindowShouldClose(window);}

private:
    void initWindow();

    const int height;
    const int width;

    std::string windowName;
    GLFWwindow *window;
};

}