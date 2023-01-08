#pragma once

#include "Quark/Core/Core.h"

extern "C" {

	typedef struct _MonoClass MonoClass;
	typedef struct _MonoImage MonoImage;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoObject MonoObject;
}

namespace Quark {

	class ScriptInstance;
	class ScriptMethod;

	class ScriptClass
	{
	public:
		ScriptClass() = default;
		ScriptClass(MonoImage* assemblyImage, std::string_view nameSpace, std::string_view className);

		/// <summary>
		/// Returns a hollow script instance.
		/// The managed object is not created after calling this function.
		/// </summary>
		/// <returns></returns>
		ScriptInstance CreateInstance() const;

		ScriptMethod   GetMethodByName(std::string_view name, int parameterCount) const;

		operator bool() const { return m_MonoClass; }

	private:
		MonoClass* m_MonoClass = nullptr;
	};

	class ScriptInstance
	{
	public:
		ScriptInstance() = default;
		ScriptInstance(MonoClass* monoClass);

		/// <summary>
		/// Invokes new and calls the default constructor.
		/// If the constructor has arguments, the function will fail.
		/// </summary>
		/// <returns></returns>
		ScriptInstance& Instanciate();

		/// <summary>
		/// Invokes new and calls the constructor with matching arguments.
		/// </summary>
		/// <param name="params"></param>
		/// <returns></returns>
		ScriptInstance& Instanciate(ScriptMethod constructor, void* params[]);

		ScriptInstance  Clone() const;

		operator bool() const { return m_Instance; }

	private:
		MonoClass* m_MonoClass = nullptr;
		MonoObject* m_Instance = nullptr;

		friend class ScriptMethod;
	};

	class ScriptMethod
	{
	public:
		ScriptMethod() = default;
		ScriptMethod(MonoClass* monoClass, std::string_view name, int parameterCount);

		/// <summary>
		/// Invokes the method bound to the instance.
		/// </summary>
		/// <param name="instance"></param>
		/// <param name="params"></param>
		void Invoke(ScriptInstance instance, void* params[] = nullptr);

		/// <summary>
		/// Invokes the static method.
		/// </summary>
		/// <param name="params"></param>
		void Invoke(void* params[] = nullptr);

		operator bool() const { return m_MonoMethod; }

	private:
		MonoMethod* m_MonoMethod = nullptr;

		friend class ScriptInstance;
	};
}
