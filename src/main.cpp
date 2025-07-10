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

	// Get path of a loaded DLL by name
	static std::string GetLoadedDLLPath(const std::string& filename)
	{
		HMODULE handle = GetModuleHandle(filename.c_str());
		char* path = new char[MAX_PATH];

		GetModuleFileName(handle, path, MAX_PATH);

		return path;
	}

	// Get CWD
	static std::string GetWorkingPath()
	{
		return std::filesystem::current_path().string();
	}

}

namespace Loader
{

	static void Init()
	{
		// Working Path
		std::string currentWorkingDirectory = Utils::GetWorkingPath();

		// UE4SS DLLs
		std::string UE4SSLoader = "dwmapi.dll";
		std::string UE4SS = currentWorkingDirectory + "\\ue4ss\\UE4SS.dll";

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
		REX::INFO("Checking for 'UE4SS.dll'");

		// Unable to find UE4SS
		if (!Utils::FileExists(UE4SS)) 
		{
			REX::CRITICAL("ERROR: Unable to find 'UE4SS.dll' is ue4ss installed correctly?");
			return;
		}
		
		REX::INFO("Found 'UE4SS.dll' at '{}'", UE4SS);
		REX::INFO("Loading 'UE4SS.dll'");

		// Attempt Load UE4SS
		if (!LoadLibrary(UE4SS.c_str())) 
		{
			std::string errorString = Utils::GetErrorMessage(GetLastError());
			REX::CRITICAL("ERROR: Unable to load 'UE4SS.dll'");
			REX::CRITICAL("{}", errorString);
			return;
		}

		REX::INFO("Successfully loaded 'UE4SS.dll'");
	}

}

// OBSE entry point
OBSE_PLUGIN_PRELOAD(OBSE::PreLoadInterface* a_obse) 
{
	OBSE::Init(a_obse, { .hook = false });
	
	// Only load if game, not editor
	if (a_obse->IsEditor() == false)
	{
		Loader::Init();
	}

	return true;
}
