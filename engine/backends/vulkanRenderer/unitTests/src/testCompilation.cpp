#include "logger.h"
#include <fstream>
#include <filesystem>
#include <gtest/gtest.h>



TEST(Vulkan, Compilation)
{
    // Can't do any meaningfull unit tests here,
    // as the vulkanRendererBackend can't be used without other systems, e.g. a window implementation.
    LOG_INFO("VulkanRendererBackend compiled succesfully!");
}