#pragma once

#include "ScriptClass.h"
#include "Quark/Core/Core.h"

#include <filesystem>

extern "C" {

	typedef struct _MonoDomain MonoDomain;
	typedef struct _MonoImage MonoImage;
	typedef struct _MonoType MonoType;
}

namespace Quark {

	class Scene;
	class Entity;

	namespace ScriptRuntime {

		void InitializeMono(const std::filesystem::path& monoDirectory);
		void ShutdownMono();

		void Initialize();
		void Shutdown();

		bool LoadAppAssembly(const std::filesystem::path& filepath);
		bool LoadCoreAssembly(const std::filesystem::path& filepath);

		bool EntityHasComponentType(Entity entity, MonoType* type);

		void OnRuntimeStart(Scene* scene);
		void OnRuntimeStop();

		Scene* GetRuntimeContext();

		MonoDomain* GetAppDomain();
		MonoImage*  GetCoreAssemblyImage();

		ScriptClass GetDefaultEntityScriptClass();
	}
}
