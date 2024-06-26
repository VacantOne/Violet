#include "vlpch.h"
#include "Violet/Core/Input.h"

#include "Violet/Core/Application.h"

#include <GLFW/glfw3.h>
#include <imgui.h>

namespace Violet {

	bool Input::IsKeyPressed(const KeyCode key)
	{
		auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, static_cast<int32_t>(key));
		
		// return state == GLFW_PRESS || state == GLFW_REPEAT;
		return state == GLFW_PRESS;
	}

	bool Input::IsMouseButtonPressed(const MouseButtonCode button)
	{
		auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}

	glm::vec2 Input::GetMousePosition()
	{
		auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}

	float Input::GetMouseX()
	{
		return GetMousePosition().x;
	}

	float Input::GetMouseY()
	{
		return GetMousePosition().y;
	}

	glm::vec2 Input::GetMouseImGuiPosition()
	{
		return Application::Get().CurrentMouse;
	}

	float Input::GetImGuiMouseX()
	{
		return GetMouseImGuiPosition().x;
	}

	float Input::GetImGuiMouseY()
	{
		return GetMouseImGuiPosition().y;
	}

	Entity Input::GetMouseHoevered()
	{
		return Application::Get().HoeveredEntity;
	}

	
}