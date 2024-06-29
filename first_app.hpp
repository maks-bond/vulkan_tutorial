#pragma once

#include "lve_window.hpp"
#include "lve_pipeline.hpp"
#include "lve_device.hpp"

namespace lve {

class FirstApp {
public:
    FirstApp();

    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    void run();

private:
    LveWindow lveWindow;
    LveDevice lveDevice{lveWindow};
    LvePipeline lve_pipeline{lveDevice, "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv", LvePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};
};

}