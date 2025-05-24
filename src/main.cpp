#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <PCH.h>

namespace Utils
{

	// Check if file exists at filepath
	static bool FileExists(const std::string& filepath)
	{
		return std::filesystem::exists(filepath);
	}

	// Get error message string from error code
	static std::string GetErrorMessage(DWORD errorCode)
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

	// Get the path a loaded DLL was loaded from
	static std::string GetLoadedDLLPath(const std::string& filename)
	{
		HMODULE handle = GetModuleHandle(filename.c_str());
		char* path = new char[MAX_PATH];

		GetModuleFileName(handle, path, MAX_PATH);

		return path;
	}

}

namespace Hooks
{

	static void Install()
	{
		// UE4SS DLLs
		std::string UE4SS_DLL = "UE4SS.dll";
		std::string UE4SSLoader_DLL = "dwmapi.dll";

		// Game Paths
		std::string GameRoot = "OblivionRemastered\\Binaries\\Win64";
		std::string UE4SSRoot = "ue4ss";
		std::string UE4SS = UE4SSRoot + "\\" + UE4SS_DLL;

		// UE4SS Paths
		std::string UE4SS_fromGameRoot = GameRoot + UE4SS;
		std::string UE4SS_fromGameExe = UE4SS;

		// Path to found DLL
		std::string dllPath;

		// Check if UE4SS is already loaded via existence of dwmapi.dll
		// If it is then don't bother trying to load any further
		std::string dwmapiLocation = Utils::GetLoadedDLLPath(UE4SSLoader_DLL);
		if (dwmapiLocation.contains("OblivionRemastered"))
		{
			REX::CRITICAL("ERROR: 'dwmapi.dll' was loaded from game root, This MUST be deleted!");
			REX::CRITICAL("{}", dwmapiLocation);
			return;
		}

		REX::INFO("Locating 'UE4SS.dll'");

		// Find UE4SS
		if (Utils::FileExists(UE4SS_fromGameExe))
		{
			dllPath = UE4SS_fromGameExe;
		}
		else if (Utils::FileExists(UE4SS_fromGameRoot))
		{
			dllPath = UE4SS_fromGameRoot;
		}
		else
		{
			REX::CRITICAL("Unable to find 'UE4SS.dll' is ue4ss installed correctly?");
		}

		// Load UE4SS if found
		if (!dllPath.empty())
		{
			REX::INFO("Found 'UE4SS.dll' at '{}'", dllPath);
			REX::INFO("Loading 'UE4SS.dll'");

			if (!LoadLibrary(dllPath.c_str()))
			{
				std::string errorString = Utils::GetErrorMessage(GetLastError());
				REX::CRITICAL("Unable to load 'UE4SS.dll' ERROR: {}", errorString);
			}
			else
			{
				REX::INFO("Successfully loaded 'UE4SS.dll'");
			}
		}
	}

}


/*
 * OBSE Entry
 */
static void MessageHandler(OBSE::MessagingInterface::Message* a_msg)
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
