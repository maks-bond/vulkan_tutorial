#include "lve_pipeline.hpp"
#include "lve_device.hpp"

#include <fstream>
#include <stdexcept>
#include <iostream>
#include <vulkan/vulkan_core.h>

namespace lve {

LvePipeline::LvePipeline(LveDevice &device, const std::string &vertFilePath, const std::string &fragFilePath, const PipelineConfigInfo &config_info) : lveDevice(device) {
    createGraphicsPipeline(vertFilePath, fragFilePath, config_info);
}

std::vector<char> LvePipeline::readFile(const std::string &file_path) {
    std::ifstream file(file_path, std::ios::ate | std::ios::binary);

    if(!file.is_open()) {
        throw std::runtime_error("failed to open file: " + file_path);
    }

    size_t file_size = static_cast<size_t>(file.tellg());

    std::vector<char> buffer(file_size);

    file.seekg(0);
    file.read(buffer.data(), file_size);

    file.close();
    return buffer;
}

void LvePipeline::createGraphicsPipeline(const std::string &vertFilePath, const std::string &fragFilePath, const PipelineConfigInfo &config_info) {
    const auto vertCode = readFile(vertFilePath);
    const auto fragCode = readFile(fragFilePath);

    std::cout<<"Vertex Shader Code Size: "<<vertCode.size()<<"\n";
    std::cout<<"Fragment Shader Code Size: "<<fragCode.size()<<"\n";
}

void LvePipeline::createShaderModule(const std::vector<char> &code, VkShaderModule *shader_module) {
    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = code.size();
    create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

    if(vkCreateShaderModule(lveDevice.device(), &create_info, nullptr, shader_module) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module");
    }
}

PipelineConfigInfo LvePipeline::defaultPipelineConfigInfo(uint32_t width, uint32_t height) {
    PipelineConfigInfo config_info;

    return config_info;
}

}