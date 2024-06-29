#pragma once

#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "lve_device.hpp"

namespace lve {

struct PipelineConfigInfo {};

class LvePipeline {
public:
    LvePipeline(LveDevice &device, const std::string &vertFilePath, const std::string &fragFilePath, const PipelineConfigInfo &config_info);
    ~LvePipeline() {}

    LvePipeline(const LvePipeline &) = delete;
    void operator=(const LvePipeline&) = delete;

    static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);

private:
    static std::vector<char> readFile(const std::string &file_path);

    void createGraphicsPipeline(const std::string &vertFilePath, const std::string &fragFilePath, const PipelineConfigInfo &config_info);

    void createShaderModule(const std::vector<char> &code, VkShaderModule *shader_module);
    
    LveDevice &lveDevice;
    VkPipeline graphicsPipeline;
    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;
};

}