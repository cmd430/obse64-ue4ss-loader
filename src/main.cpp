#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <PCH.h>

namespace Hooks
{

	static bool FileExists(const std::string& filename)
	{
		return std::filesystem::exists(filename);
	}

	static std::string ErrorMessage(DWORD errorCode)
	{
		std::string message;
		LPVOID lpMsgBuf = nullptr;

		if (FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
					FORMAT_MESSAGE_FROM_SYSTEM |
					FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				errorCode,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR)&lpMsgBuf,
				0,
				NULL) == 0)
		{
			message = "Unable to parse error";
		}
		message = (LPCTSTR)lpMsgBuf;
		LocalFree(lpMsgBuf);

		return message;
	}

	static void Install()
	{
		REX::INFO("Finding 'UE4SS.dll'");

		std::string fromGameRoot = "OblivionRemastered\\Binaries\\Win64\\ue4ss\\UE4SS.dll";
		std::string fromGameExe = "ue4ss\\UE4SS.dll";
		std::string dllPath;

		if (FileExists(fromGameExe))
		{
			dllPath = fromGameExe;
		}
		else if (FileExists(fromGameRoot))
		{
			dllPath = fromGameRoot;
		}
		else
		{
			REX::CRITICAL("Unable to find 'UE4SS.dll' is ue4ss installed correctly?");
		}

		if (!dllPath.empty())
		{
			REX::INFO("Found 'UE4SS.dll' at '{}'", dllPath);
			REX::INFO("Loading 'UE4SS.dll'");

			if (!LoadLibrary(dllPath.c_str()))
			{
				REX::CRITICAL("Unable to load 'UE4SS.dll' error: {}", ErrorMessage(GetLastError()));
			}
			else
			{
				REX::INFO("Successfully loaded 'UE4SS.dll'");
			}
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
