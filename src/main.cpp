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

		if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL) == 0)
		{
			message = "Unable to parse error";
		}
		else
		{
			message = (LPCTSTR)lpMsgBuf;
		}

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

	static std::string GetBasePath() {
		char* path = new char[MAX_PATH];

		GetModuleFileName(NULL, path, MAX_PATH);
		std::filesystem::path fullpath(path);

		return fullpath.remove_filename().string();
	}

	static std::string GetWorkingPath()
	{
		return std::filesystem::current_path().string();
	}

}

namespace Hooks
{

	static void Install()
	{
		// Working Paths
		std::string currentWorkingDirectory = Utils::GetWorkingPath();
		std::string basePath = Utils::GetBasePath();

		// UE4SS DLLs
		std::string UE4SSLoader = "dwmapi.dll";
		std::string UE4SS = basePath + "ue4ss\\UE4SS.dll";

		// Check if UE4SS is already loaded via existence of dwmapi.dll (in the game root)
		// If it is then don't bother trying to load any further
		std::string dwmapiLocation = Utils::GetLoadedDLLPath(UE4SSLoader);
		if (dwmapiLocation.contains("OblivionRemastered"))
		{
			REX::CRITICAL("ERROR: 'dwmapi.dll' was loaded from game root, This MUST be deleted!");
			REX::CRITICAL("{}", dwmapiLocation);
			return;
		}

		REX::INFO("Current directory is '{}'", currentWorkingDirectory);
		REX::INFO("Base path is '{}'", basePath);
		REX::INFO("Checking for 'UE4SS.dll'");

		// Unable to find UE4SS
		if (!Utils::FileExists(UE4SS)) 
		{
			REX::CRITICAL("Unable to find 'UE4SS.dll' is ue4ss installed correctly?");
			return;
		}
		
		REX::INFO("Found 'UE4SS.dll' at '{}'", UE4SS);
		REX::INFO("Loading 'UE4SS.dll'");

		// Attempt Load UE4SS
		if (!LoadLibrary(UE4SS.c_str()))
		{
			std::string errorString = Utils::GetErrorMessage(GetLastError());
			REX::CRITICAL("Unable to load 'UE4SS.dll' ERROR: {}", errorString);
			return;
		}

		REX::INFO("Successfully loaded 'UE4SS.dll'");
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
