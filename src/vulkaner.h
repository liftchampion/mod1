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

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>

class Vulkaner {
public:
	inline Vulkaner(int width, int height)
		: _width(width)
		, _height(height)
	{}
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

protected:
	void initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		_window = glfwCreateWindow(_width, _height, "Vulkan window", nullptr,nullptr);
	}

	void initVulkan() {

	}

	void mainLoop() {

	}

	void cleanup() {

	}

	int _width;
	int _height;
	GLFWwindow *_window;
};
