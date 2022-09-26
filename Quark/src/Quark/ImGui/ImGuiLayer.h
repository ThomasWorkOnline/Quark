#pragma once

#include <Quark/Core/Layer.h>

namespace Quark {

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer(Application* app);
		virtual ~ImGuiLayer() override;

		void BeginFrame();
		void EndFrame();

	private:
		void SetStyleDarkTheme();
	};
}
