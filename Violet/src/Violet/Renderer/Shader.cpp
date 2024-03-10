#include "vlpch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Violet {

	Shader* Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc) {
		switch (Renderer::GetAPI()) {
			case RendererAPI::API::None:
				VL_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:
				return new OpenGLShader(vertexSrc, fragmentSrc);
		}

		VL_CORE_ASSERT(false, "δ֪����ȾAPI(Unkown RendererAPI)");
		return nullptr;
	}

}