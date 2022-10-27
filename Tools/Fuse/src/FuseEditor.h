#pragma once

#include <Quark.h>

namespace Quark {

	class FuseEditor : public Application
	{
	public:
		FuseEditor(const ApplicationSpecification& spec);

		virtual void OnUpdate(Timestep ts) final override;
		virtual void OnRender() final override;
	};
}
