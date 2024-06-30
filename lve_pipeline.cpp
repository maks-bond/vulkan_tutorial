#include "lve_pipeline.hpp"
#include "lve_device.hpp"

#include <cstddef>
#include <fstream>
#include <stdexcept>
#include <cassert>
#include <iostream>
#include <vulkan/vulkan_core.h>

namespace lve {

LvePipeline::LvePipeline(LveDevice &device, const std::string &vertFilePath, const std::string &fragFilePath, const PipelineConfigInfo &config_info) : lveDevice(device) {
    createGraphicsPipeline(vertFilePath, fragFilePath, config_info);
}

LvePipeline::~LvePipeline() {
    vkDestroyShaderModule(lveDevice.device(), vertShaderModule, nullptr);
    vkDestroyShaderModule(lveDevice.device(), fragShaderModule, nullptr);
    vkDestroyPipeline(lveDevice.device(), graphicsPipeline, nullptr);
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
    assert(config_info.pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline: no pipelineLayout provided in configInfo");
    assert(config_info.renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline: no renderPass provided in configInfo");

    const auto vertCode = readFile(vertFilePath);
    const auto fragCode = readFile(fragFilePath);

    createShaderModule(vertCode, &vertShaderModule);
    createShaderModule(fragCode, &fragShaderModule);

    VkPipelineShaderStageCreateInfo shaderStages[2];
    shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].module = vertShaderModule;
    shaderStages[0].pName = "main";
    shaderStages[0].flags = 0;
    shaderStages[0].pNext = nullptr;
    shaderStages[0].pSpecializationInfo = nullptr;

    shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module = fragShaderModule;
    shaderStages[1].pName = "main";
    shaderStages[1].flags = 0;
    shaderStages[1].pNext = nullptr;
    shaderStages[1].pSpecializationInfo = nullptr;

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr;
    vertexInputInfo.pVertexBindingDescriptions = nullptr;


    VkPipelineViewportStateCreateInfo viewportInfo{};
    viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportInfo.viewportCount = 1;
    viewportInfo.pViewports = &config_info.viewport;
    viewportInfo.scissorCount = 1;
    viewportInfo.pScissors = &config_info.scissor;

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &config_info.inputAssemblyInfo;
    pipelineInfo.pViewportState = &viewportInfo;
    pipelineInfo.pRasterizationState = &config_info.rasterizationInfo;
    pipelineInfo.pMultisampleState = &config_info.multisampleInfo;
    pipelineInfo.pColorBlendState = &config_info.colorBlendInfo;
    pipelineInfo.pDepthStencilState = &config_info.depthStencilInfo;
    pipelineInfo.pDynamicState = nullptr;

    pipelineInfo.layout = config_info.pipelineLayout;
    pipelineInfo.renderPass = config_info.renderPass;
    pipelineInfo.subpass = config_info.subpass;

    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    if(vkCreateGraphicsPipelines(lveDevice.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline");
    }
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
    config_info.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    config_info.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    config_info.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    config_info.viewport.x = 0.0f;
    config_info.viewport.y = 0.0f;
    config_info.viewport.width = static_cast<float>(width);
    config_info.viewport.height = static_cast<float>(height);
    config_info.viewport.minDepth = 0.0f;
    config_info.viewport.maxDepth = 1.0f;
    
    config_info.scissor.offset = {0, 0};
    config_info.scissor.extent = {width, height};

    config_info.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    config_info.rasterizationInfo.depthClampEnable = VK_FALSE;
    config_info.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
    config_info.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
    config_info.rasterizationInfo.lineWidth = 1.0f;
    config_info.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
    config_info.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    config_info.rasterizationInfo.depthBiasEnable = VK_FALSE;
    config_info.rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
    config_info.rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
    config_info.rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional

    config_info.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    config_info.multisampleInfo.sampleShadingEnable = VK_FALSE;
    config_info.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    config_info.multisampleInfo.minSampleShading = 1.0f;           // Optional
    config_info.multisampleInfo.pSampleMask = nullptr;             // Optional
    config_info.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
    config_info.multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional

    config_info.colorBlendAttachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
      VK_COLOR_COMPONENT_A_BIT;
    config_info.colorBlendAttachment.blendEnable = VK_FALSE;
    config_info.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
    config_info.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
    config_info.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
    config_info.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
    config_info.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
    config_info.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

    config_info.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    config_info.colorBlendInfo.logicOpEnable = VK_FALSE;
    config_info.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
    config_info.colorBlendInfo.attachmentCount = 1;
    config_info.colorBlendInfo.pAttachments = &config_info.colorBlendAttachment;
    config_info.colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
    config_info.colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
    config_info.colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
    config_info.colorBlendInfo.blendConstants[3] = 0.0f;  // Optional
  
    config_info.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    config_info.depthStencilInfo.depthTestEnable = VK_TRUE;
    config_info.depthStencilInfo.depthWriteEnable = VK_TRUE;
    config_info.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    config_info.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
    config_info.depthStencilInfo.minDepthBounds = 0.0f;  // Optional
    config_info.depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
    config_info.depthStencilInfo.stencilTestEnable = VK_FALSE;
    config_info.depthStencilInfo.front = {};  // Optional
    config_info.depthStencilInfo.back = {};   // Optional

    return config_info;
}

}