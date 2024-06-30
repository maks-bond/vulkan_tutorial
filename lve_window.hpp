#pragma once

#include <cstdint>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace lve {
class LveWindow {
public:
    LveWindow(uint32_t w, uint32_t h, const std::string &name);
    ~LveWindow();
    LveWindow(const LveWindow &) = delete;
    LveWindow &operator=(const LveWindow &) = delete;
    bool shouldClose() {return glfwWindowShouldClose(window);}

    void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

    VkExtent2D get_extent() {return {width, height} };

private:
    void initWindow();

    const uint32_t height;
    const uint32_t width;

    std::string windowName;
    GLFWwindow *window;
};

}