#include "qkpch.h"
#include "ScriptClass.h"

#include "ScriptRuntime.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

namespace Quark {

	/////////////////////////////////////////////////////////////
	/// Script Class
	///

	ScriptClass::ScriptClass(MonoImage* assemblyImage, std::string_view nameSpace, std::string_view className)
		: m_MonoClass(mono_class_from_name(assemblyImage, nameSpace.data(), className.data()))
	{
	}

	ScriptInstance ScriptClass::CreateInstance() const
	{
		ScriptInstance instance(m_MonoClass);
		return instance;
	}

	ScriptMethod ScriptClass::GetMethodByName(std::string_view name, int parameterCount) const
	{
		ScriptMethod method(m_MonoClass, name, parameterCount);
		return method;
	}

	/////////////////////////////////////////////////////////////
	/// Script Instance
	///

	ScriptInstance::ScriptInstance(MonoClass* monoClass)
		: m_MonoClass(monoClass)
	{
	}

	ScriptInstance& ScriptInstance::Instanciate()
	{
		m_Instance = mono_object_new(ScriptRuntime::GetAppDomain(), m_MonoClass);
		mono_runtime_object_init(m_Instance);
		return *this;
	}

	ScriptInstance& ScriptInstance::Instanciate(ScriptMethod constructor, void* params[])
	{
		m_Instance = mono_object_new(ScriptRuntime::GetAppDomain(), m_MonoClass);
		mono_runtime_invoke(constructor.m_MonoMethod, m_Instance, params, nullptr);

		return *this;
	}

	ScriptInstance ScriptInstance::Clone() const
	{
		ScriptInstance clone(m_MonoClass);
		clone.m_Instance = mono_object_clone(m_Instance);
		return clone;
	}

	/////////////////////////////////////////////////////////////
	/// Script Method
	///

	ScriptMethod::ScriptMethod(MonoClass* monoClass, std::string_view name, int parameterCount)
		: m_MonoMethod(mono_class_get_method_from_name(monoClass, name.data(), parameterCount))
	{
	}

	void ScriptMethod::Invoke(ScriptInstance instance, void* params[])
	{
		QK_CORE_ASSERT(instance.m_Instance, "Cannot invoke a method on a nullptr instance!");
		QK_CORE_ASSERT(m_MonoMethod, "Invoking a method that doesn't exist!");
		mono_runtime_invoke(m_MonoMethod, instance.m_Instance, params, nullptr);
	}

	void ScriptMethod::Invoke(void* params[])
	{
		QK_CORE_ASSERT(m_MonoMethod, "Invoking a method that doesn't exist!");
		mono_runtime_invoke(m_MonoMethod, nullptr, params, nullptr);
	}
}
