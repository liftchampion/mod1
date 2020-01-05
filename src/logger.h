//
// Created by Griffin king Gerardys on 05/01/2020.
//

#include "vulkaner.h"
#include <map>

#pragma once

#define KGRN   "\033[0;32;32m"
#define KCYN   "\033[0;36m"
#define KRED   "\033[0;32;31m"
#define KYEL   "\033[1;33m"
#define KBLU   "\033[0;32;34m"
#define KCYN_L "\033[1;36m"
#define KBRN   "\033[0;33m"
#define RESET  "\033[0m"

#define VULKAN_TRY(func) \
	std::cout << __FILE__ << ':' << __LINE__ << "| " << KBLU #func RESET << ' '; \
    auto res = func; \
	if (res != VkResult::VK_SUCCESS) { \
		std::cout << KRED "ERROR: " RESET << res << " [" <<  g_vulkan_errors[res] << ']' << std::endl; \
		exit(0); \
	} else { \
		std::cout << KGRN "SUCCESS: " RESET << res << " [" <<  g_vulkan_errors[res] << ']' << std::endl; \
	}

static std::map<int, std::string> g_vulkan_errors = {
		{0,                                              "VK_SUCCESS"},
		{1,                                              "VK_NOT_READY"},
		{2,                                              "VK_TIMEOUT"},
		{3,                                              "VK_EVENT_SET"},
		{4,                                              "VK_EVENT_RESET"},
		{5,                                              "VK_INCOMPLETE"},
		{-1,                                             "VK_ERROR_OUT_OF_HOST_MEMORY"},
		{-2,                                             "VK_ERROR_OUT_OF_DEVICE_MEMORY"},
		{-3,                                             "VK_ERROR_INITIALIZATION_FAILED"},
		{-4,                                             "VK_ERROR_DEVICE_LOST"},
		{-5,                                             "VK_ERROR_MEMORY_MAP_FAILED"},
		{-6,                                             "VK_ERROR_LAYER_NOT_PRESENT"},
		{-7,                                             "VK_ERROR_EXTENSION_NOT_PRESENT"},
		{-8,                                             "VK_ERROR_FEATURE_NOT_PRESENT"},
		{-9,                                             "VK_ERROR_INCOMPATIBLE_DRIVER"},
		{-10,                                            "VK_ERROR_TOO_MANY_OBJECTS"},
		{-11,                                            "VK_ERROR_FORMAT_NOT_SUPPORTED"},
		{-12,                                            "VK_ERROR_FRAGMENTED_POOL"},
		{-1000069000,                                    "VK_ERROR_OUT_OF_POOL_MEMORY"},
		{-1000072003,                                    "VK_ERROR_INVALID_EXTERNAL_HANDLE"},
		{-1000000000,                                    "VK_ERROR_SURFACE_LOST_KHR"},
		{-1000000001,                                    "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR"},
		{1000001003,                                     "VK_SUBOPTIMAL_KHR"},
		{-1000001004,                                    "VK_ERROR_OUT_OF_DATE_KHR"},
		{-1000003001,                                    "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR"},
		{-1000011001,                                    "VK_ERROR_VALIDATION_FAILED_EXT"},
		{-1000012000,                                    "VK_ERROR_INVALID_SHADER_NV"},
		{-1000158000,                                    "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT"},
		{-1000161000,                                    "VK_ERROR_FRAGMENTATION_EXT"},
		{-1000174001,                                    "VK_ERROR_NOT_PERMITTED_EXT"},
		{-1000244000,                                    "VK_ERROR_INVALID_DEVICE_ADDRESS_EXT"},
		{-1000255000,                                    "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT"},
		{VK_ERROR_OUT_OF_POOL_MEMORY,                    "VK_ERROR_OUT_OF_POOL_MEMORY_KHR"},
		{VK_ERROR_INVALID_EXTERNAL_HANDLE,               "VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR"},
		{VK_ERROR_FRAGMENTED_POOL,                       "VK_RESULT_BEGIN_RANGE"},
		{VK_INCOMPLETE,                                  "VK_RESULT_END_RANGE"},
		{(VK_INCOMPLETE - VK_ERROR_FRAGMENTED_POOL + 1), "VK_RESULT_RANGE_SIZE"},
		{0x7FFFFFFF,                                     "VK_RESULT_MAX_ENUM"},
};