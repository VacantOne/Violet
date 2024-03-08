#pragma once

#include "Core.h"
#include "Events//Event.h"
#include "Window.h"
#include "Violet/Layer.h"
#include "Violet/LayerStack.h"
#include "Violet/Events/ApplicationEvent.h"

#include "Violet/ImGui/ImGuiLayer.h"

#include "Violet/Renderer/Shader.h"
#include "Violet/Renderer/Buffer.h"
#include "Violet/Renderer/VertexArray.h"

namespace Violet {
	class VIOLET_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }

		inline static Application& Get() { return *s_Instance; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		
		ImGuiLayer* m_ImGuiLayer;

		LayerStack m_LayerStack;
		/*
		unsigned int m_VertexArray;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
		*/
		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<VertexArray> m_VertexArray;

		std::shared_ptr<Shader> m_BlueShader;
		std::shared_ptr<VertexArray> m_SquareVA;
	private:
		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();

}
