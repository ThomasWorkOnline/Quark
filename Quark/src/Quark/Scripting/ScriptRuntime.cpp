#include "qkpch.h"
#include "ScriptRuntime.h"

#include "ScriptClass.h"
#include "InternalCalls.h"

#include "Quark/Scene/Scene.h"
#include "Quark/Scene/Entity.h"
#include "Quark/Scene/Components.h"
#include "Quark/Filesystem/Filesystem.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/reflection.h>
#include <mono/metadata/threads.h>

#include <functional>
#include <unordered_map>

namespace Quark {

	struct ScriptRuntimeData
	{
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		MonoAssembly* CoreAssembly = nullptr;
		MonoImage* CoreAssemblyImage = nullptr;

		MonoAssembly* AppAssembly = nullptr;
		MonoImage* AppAssemblyImage = nullptr;

		Scene* RuntimeContext = nullptr;
		ScriptClass BaseEntityClass;

		std::unordered_map<MonoType*, std::function<bool(Entity)>> EntityHasComponentFuncs;
	};

	static ScriptRuntimeData s_Data;

	template<typename... Component>
	static void RegisterComponent()
	{
		([]()
		{
			std::string_view typeName = typeid(Component).name();
			size_t pos = typeName.find_last_of(':');
			std::string_view structName = typeName.substr(pos + 1);
			std::string managedTypename = fmt::format("Quark.{0}", structName);

			MonoType* managedType = mono_reflection_type_from_name(managedTypename.data(), s_Data.CoreAssemblyImage);
			if (!managedType)
			{
				QK_CORE_ERROR("Could not find component type {0}", managedTypename);
				return;
			}

			s_Data.EntityHasComponentFuncs[managedType] = [](Entity entity) { return entity.HasComponent<Component>(); };
		}(), ...);
	}

	template<typename... Component>
	static void RegisterComponent(ComponentGroup<Component...>)
	{
		RegisterComponent<Component...>();
	}

	static void RegisterComponents()
	{
		using RegisteredComponents = ComponentGroup<Transform3DComponent, PhysicsComponent>;

		s_Data.EntityHasComponentFuncs.clear();
		RegisterComponent(RegisteredComponents{});
	}

	void ScriptRuntime::InitializeMono(const std::filesystem::path& monoDirectory)
	{
		QK_PROFILE_FUNCTION();

		mono_set_assemblies_path((monoDirectory / "mono/lib").string().c_str());

		MonoDomain* rootDomain = mono_jit_init("QuarkScriptCoreRuntime");
		QK_CORE_ASSERT(rootDomain, "Could not initialize mono root domain!");

		s_Data.RootDomain = rootDomain;

		mono_thread_set_main(mono_thread_current());
	}
	
	void ScriptRuntime::ShutdownMono()
	{
		QK_PROFILE_FUNCTION();

		mono_domain_set(s_Data.RootDomain, false);

		mono_domain_unload(s_Data.AppDomain);
		s_Data.AppDomain = nullptr;

		mono_jit_cleanup(s_Data.RootDomain);
		s_Data.RootDomain = nullptr;

		s_Data.CoreAssembly = nullptr;
		s_Data.CoreAssemblyImage = nullptr;

		s_Data.AppAssembly = nullptr;
		s_Data.AppAssemblyImage = nullptr;
	}

	static MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath)
	{
		QK_PROFILE_FUNCTION();

		std::vector<uint8_t> imageBytes = Filesystem::ReadBinaryFile(assemblyPath.string());

		MonoImageOpenStatus status;
		MonoImage* image = mono_image_open_from_data_full((char*)imageBytes.data(), (uint32_t)imageBytes.size(), true, &status, false);

		if (status != MONO_IMAGE_OK)
		{
			const char* errorMessage = mono_image_strerror(status);
			QK_CORE_ERROR("Failed to load CSharp assembly: {0}", errorMessage);
			return nullptr;
		}

		MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.string().c_str(), &status, false);
		mono_image_close(image);

		return assembly;
	}

	static void PrintAssemblyInfo()
	{
		const MonoTableInfo* typeDefinitionTable = mono_image_get_table_info(s_Data.CoreAssemblyImage, MONO_TABLE_TYPEDEF);

		int numTypes = mono_table_info_get_rows(typeDefinitionTable);
		for (int i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(s_Data.CoreAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(s_Data.CoreAssemblyImage, cols[MONO_TYPEDEF_NAME]);

			QK_CORE_WARN("{0}.{1}", nameSpace, name);
		}
	}

	void ScriptRuntime::Initialize()
	{
		QK_PROFILE_FUNCTION();

		auto& coreDirectory = GetRuntimeDirectory();
		LoadCoreAssembly((coreDirectory / "Resources/Scripts/Quark-ScriptCore.dll").string());
		RegisterComponents();

		InternalCalls::RegisterInternalCalls();

		s_Data.BaseEntityClass = ScriptClass(s_Data.CoreAssemblyImage, "Quark", "Entity");

#if 0
		ScriptClass scriptClass(s_Data.CoreAssemblyImage, "Sandbox", "Player");

		// Create object
		ScriptInstance instance = scriptClass.CreateInstance().Instanciate();

		ScriptMethod printMessage = scriptClass.GetMethodByName("PrintMessage", 0);
		printMessage.Invoke(instance);

		{
			int value = 13;
			void* params[] = {
				&value
			};

			ScriptMethod printInt = scriptClass.GetMethodByName("PrintInt", 1);
			printInt.Invoke(instance, params);
		}

		{
			void* params[] = {
				mono_string_new(s_Data.AppDomain, "Hello from C++")
			};

			ScriptMethod printCustomMessage = scriptClass.GetMethodByName("PrintCustomMessage", 1);
			printCustomMessage.Invoke(instance, params);
		}
#endif
	}

	void ScriptRuntime::Shutdown()
	{
	}

	bool ScriptRuntime::LoadAppAssembly(const std::filesystem::path& filepath)
	{
		s_Data.AppAssembly = LoadMonoAssembly(filepath);

		if (!s_Data.AppAssembly)
			return false;

		s_Data.AppAssemblyImage = mono_assembly_get_image(s_Data.AppAssembly);
		return true;
	}

	bool ScriptRuntime::LoadCoreAssembly(const std::filesystem::path& filepath)
	{
		char appDomainName[] = "QuarkScriptRuntime";

		s_Data.AppDomain = mono_domain_create_appdomain(appDomainName, nullptr);
		mono_domain_set(s_Data.AppDomain, true);

		s_Data.CoreAssembly = LoadMonoAssembly(filepath);

		if (!s_Data.CoreAssembly)
			return false;

		s_Data.CoreAssemblyImage = mono_assembly_get_image(s_Data.CoreAssembly);
		return true;
	}

	bool ScriptRuntime::EntityHasComponentType(Entity entity, MonoType* type)
	{
		QK_CORE_ASSERT(s_Data.EntityHasComponentFuncs.find(type) != s_Data.EntityHasComponentFuncs.end(), "Component type was never registered!");
		return s_Data.EntityHasComponentFuncs.at(type)(entity);
	}

	void ScriptRuntime::OnRuntimeStart(Scene* scene)
	{
		s_Data.RuntimeContext = scene;
	}

	void ScriptRuntime::OnRuntimeStop()
	{
		s_Data.RuntimeContext = nullptr;
	}

	Scene* ScriptRuntime::GetRuntimeContext()
	{
		return s_Data.RuntimeContext;
	}

	MonoDomain* ScriptRuntime::GetAppDomain()
	{
		return s_Data.AppDomain;
	}

	MonoImage* ScriptRuntime::GetCoreAssemblyImage()
	{
		return s_Data.CoreAssemblyImage;
	}

	ScriptClass ScriptRuntime::GetDefaultEntityScriptClass()
	{
		return s_Data.BaseEntityClass;
	}
}
