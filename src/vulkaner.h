/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vulkaner.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ggerardy <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/03 23:00:24 by ggerardy          #+#    #+#             */
/*   Updated: 2020/01/03 23:00:24 by ggerardy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#define GLFW_INCLUDE_VULKAN
#define VK_USE_PLATFORM_MACOS_MVK

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <vector>
#include <set>

#include "logger.h"

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class Vulkaner {
public:
	inline Vulkaner(int width, int height)
		: _width(width)
		, _height(height)
	{}
	inline ~Vulkaner() {
		cleanup();
	}

	void init() {
		init_window();
		init_vulkan();
	}

	void run() {
		main_loop();
	}

protected:

	struct QueueFamilyIndices {
		uint32_t graphicsFamily;
		uint32_t presentFamily;

		bool isComplete() {
			return graphicsFamily != -1u;
		}
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	void init_window() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		_window = glfwCreateWindow(_width, _height, "Vulkan window", nullptr,nullptr);
	}

	void init_vulkan() {
		create_instance();
		print_extensions();
		setup_debug_messenger();
		createSurface();
		pick_physical_device();
		create_logical_device();
		get_queue();
		create_swap_chain();
	}

	void create_instance() {
		VkApplicationInfo appInfo = {};
		appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName   = "Hello Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName        = "No Engine";
		appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion         = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo         = {};
		uint32_t             glfwExtensionCount = 0;
		const char**         glfwExtensions     = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		auto extensions                         = getRequiredExtensions();

		createInfo.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();
		createInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo        = &appInfo;
		createInfo.enabledExtensionCount   = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;
		createInfo.enabledLayerCount       = 0;
		VULKAN_TRY(vkCreateInstance(&createInfo, nullptr, &_instance));
	}

	std::vector<const char*> getRequiredExtensions() {
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (enableValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}
		extensions.insert(extensions.end(), _required_device_extensions.begin(), _required_device_extensions.end());

		std::cout << "Enabled extensions:" << std::endl;
		for (const auto& ext : extensions) {
			std::cout << '\t' << ext << std::endl;
		}

		return extensions;
	}

	static void print_extensions() {
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
		std::cout << "available extensions:" << std::endl;

		for (const auto& extension : extensions) {
			std::cout << "\t" << extension.extensionName << std::endl;
		}
	}

	void setup_debug_messenger() {
		// TODO
	}

	void createSurface() {
		VULKAN_TRY(glfwCreateWindowSurface(_instance, _window, nullptr, &_surface));
	}

	void pick_physical_device() {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);
		if (deviceCount == 0) {
			std::cout << KRED "!!! NO GPU'S !!!" RESET << std::endl;
			exit(0);
		}
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());
		for (const auto& device : devices) {
			if (is_device_suitable(device)) {
				_physical_device = device;
				break;
			}
		}
		if (_physical_device == VK_NULL_HANDLE) {
			std::cout << KRED "!!! NO SUITABLE GPU !!!" RESET << std::endl;
			exit(0);
		}
	}

	bool is_device_suitable(VkPhysicalDevice device) {
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
		QueueFamilyIndices indices = find_queue_families(device);

		bool extensionsSupported = check_device_extension_support(device);
		bool swapChainAdequate = false;
		if (extensionsSupported) {
			SwapChainSupportDetails swapChainSupport = query_swap_chain_support(device);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		std::cout << "Checking GPU " << deviceProperties.deviceName << " ... ";
		bool is_ok = deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
					 && indices.isComplete()
					 && extensionsSupported
					 && swapChainAdequate;
		std::cout << (is_ok ? "OK" : "BAD") << std::endl;
		return is_ok;
	}

	bool check_device_extension_support(VkPhysicalDevice device) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(_required_device_extensions.begin(), _required_device_extensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	QueueFamilyIndices find_queue_families(VkPhysicalDevice device) {
		QueueFamilyIndices indices = {-1u, -1u};

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
			}
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _surface, &presentSupport);

			if (presentSupport) {
				indices.presentFamily = i;
			}
			if (indices.isComplete()) {
				break;
			}
			i++;
		}
		return indices;
	}

	SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice device) {
		SwapChainSupportDetails details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, nullptr);

		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, details.presentModes.data());
		}
		return details;
	}

	void create_logical_device() {
		QueueFamilyIndices indices = find_queue_families(_physical_device);

		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
		queueCreateInfo.queueCount       = 1;
		queueCreateInfo.pQueuePriorities = &_queue_priority;

		VkPhysicalDeviceFeatures deviceFeatures = {};

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType                = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos    = &queueCreateInfo;
		createInfo.queueCreateInfoCount = 1;
		createInfo.pEnabledFeatures     = &deviceFeatures;
		VULKAN_TRY(vkCreateDevice(_physical_device, &createInfo, nullptr, &_logical_device) != VK_SUCCESS);
	}

	void get_queue() {
		QueueFamilyIndices indices = find_queue_families(_physical_device);
		vkGetDeviceQueue(_logical_device, indices.graphicsFamily, 0, &_graphics_queue);
	}

	void create_swap_chain() {
		SwapChainSupportDetails swapChainSupport = query_swap_chain_support(_physical_device);

		VkSurfaceFormatKHR surfaceFormat = choose_swap_surface_format(swapChainSupport.formats);
		VkPresentModeKHR   presentMode   = choose_swap_present_mode(swapChainSupport.presentModes);
		VkExtent2D         extent        = chooseSwapExtent(swapChainSupport.capabilities);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface          = _surface;
		createInfo.minImageCount    = imageCount;
		createInfo.imageFormat      = surfaceFormat.format;
		createInfo.imageColorSpace  = surfaceFormat.colorSpace;
		createInfo.imageExtent      = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		QueueFamilyIndices indices = find_queue_families(_physical_device);
		uint32_t queueFamilyIndices[] = {indices.graphicsFamily, indices.presentFamily};

		if (indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		} else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0; // Optional
			createInfo.pQueueFamilyIndices = nullptr; // Optional
		}
		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		VULKAN_TRY(vkCreateSwapchainKHR(_logical_device, &createInfo, nullptr, &_swap_chain));
	}

	VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}
		return availableFormats[0];
	}

	VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != UINT32_MAX) {
			return capabilities.currentExtent;
		} else {
			VkExtent2D actualExtent = {(uint32_t)_width, (uint32_t)_height};

			actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

			return actualExtent;
		}
	}

	void main_loop() {
		while (!glfwWindowShouldClose(_window)) {
			glfwPollEvents();
		}
	}

	void cleanup() {
		vkDestroySwapchainKHR(_logical_device, _swap_chain, nullptr);
		vkDestroyDevice(_logical_device, nullptr);
		vkDestroySurfaceKHR(_instance, _surface, nullptr);
		vkDestroyInstance(_instance, nullptr);
		glfwDestroyWindow(_window);
		glfwTerminate();
	}

	int _width;
	int _height;

	GLFWwindow*      _window;
	VkInstance       _instance;
	VkPhysicalDevice _physical_device = VK_NULL_HANDLE;
	VkDevice         _logical_device;
	VkQueue          _graphics_queue;
	VkSurfaceKHR     _surface;
	VkSwapchainKHR   _swap_chain;

	const std::vector<const char*> _required_device_extensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	float _queue_priority = 1.0f;
};
