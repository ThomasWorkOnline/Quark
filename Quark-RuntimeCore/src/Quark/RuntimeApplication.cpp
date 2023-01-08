#include "qkpch.h"
#include "RuntimeApplication.h"

#include <Quark/EntryPoint.h>

namespace Quark {

	static bool s_Running = false;
	static bool s_Crashed = false;

	static CommandLineArguments s_CommandLineArgs;
	static std::filesystem::path s_RuntimeDirectory;

	static std::filesystem::path FindRuntimeDirectory(CommandLineArguments args)
	{
		std::filesystem::path runtimeDirectory;

		for (int i = 0; i < args.Argc; i++)
		{
			std::string_view argv = args.Argv[i];
			std::string_view runtimePathArgToken = "--runtime-path=";

			size_t pos = argv.find(runtimePathArgToken);
			if (pos != std::string::npos)
			{
				std::string_view relativePathString = argv.substr(pos + runtimePathArgToken.size());
				return std::filesystem::absolute(relativePathString);
			}
		}

		return std::filesystem::current_path();
	}

	static void LaunchApplication(CommandLineArguments args)
	{
		Application* app = nullptr;

		try
		{
			QK_BEGIN_PROFILE_SESSION("startup.json");
			app = Quark::CreateApplication(args);
			QK_END_PROFILE_SESSION();

			QK_BEGIN_PROFILE_SESSION("runtime.json");
			app->Run();
			QK_END_PROFILE_SESSION();

			QK_BEGIN_PROFILE_SESSION("shutdown.json");
			delete app;
			QK_END_PROFILE_SESSION();
		}
		catch (std::exception& e)
		{
			QK_CORE_ERROR(e.what());

			if (app)
			{
				app->Crash(e);
				delete app;
			}

			s_Crashed = true;
		}
	}

	int Main(int argc, char** argv)
	{
		s_CommandLineArgs = { argc, argv };
		s_RuntimeDirectory = FindRuntimeDirectory(s_CommandLineArgs);

		RuntimeCore::Init();

		do
		{
			LaunchApplication(s_CommandLineArgs);
		}
		while (s_Running && !s_Crashed);

		RuntimeCore::Shutdown();

		return EXIT_SUCCESS;
	}

	void SetRestartPolicy(ApplicationRestartPolicy policy)
	{
		s_Running = policy == ApplicationRestartPolicy::Restart;
	}

	CommandLineArguments GetCommandLineArguments()
	{
		return s_CommandLineArgs;
	}

	const std::filesystem::path& GetRuntimeDirectory()
	{
		return s_RuntimeDirectory;
	}
}
