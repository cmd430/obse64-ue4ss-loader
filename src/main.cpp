#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace Hooks
{

	static bool FileExists(const std::string& filename)
	{
		return std::filesystem::exists(filename);
	}


	static void Install()
	{
		REX::INFO("Loading UE4SS.dll");

		const char* fromRoot = "OblivionRemastered\\Binaries\\Win64\\ue4ss\\UE4SS.dll";
		const char* fromExe = "ue4ss\\UE4SS.dll";

		if (FileExists(fromExe))
		{
			REX::INFO("Loading from {}", fromExe);
			LoadLibrary(fromExe);
		}
		else if (FileExists(fromRoot))
		{
			REX::INFO("Loading from {}", fromRoot);
			LoadLibrary(fromRoot);
		}
		else
		{
			REX::WARN("Unable to load UE4SS.dll");
		}
	}
}

void MessageHandler(OBSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type)
	{
	case OBSE::MessagingInterface::kPostLoad:
		Hooks::Install();
		break;
	default:
		break;
	}
}

OBSE_PLUGIN_LOAD(OBSE::LoadInterface* a_obse)
{
	OBSE::Init(a_obse);
	OBSE::GetMessagingInterface()->RegisterListener(MessageHandler);
	return true;
}
