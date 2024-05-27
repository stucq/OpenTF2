#include "main.hh"

#include <stdio.h>
#include <stdlib.h>

#include <windows.h>

/**
 * Gets the base directory for a given path.
 *
 * @param pszPath The path to get the base directory of.
 * @returns The directory pszPath is in.
 */
static inline char *GetBaseDir(const char *pszPath)
{
	static char szBaseDir[MAX_PATH];

	char szBuffer[MAX_PATH];
	char *pchCurSlash;

	size_t cchLen;

	pchCurSlash = NULL;

	// End the string at the last '\\'.
	strcpy(szBuffer, pszPath);
	pchCurSlash = strrchr(szBuffer, '\\');

	if (pchCurSlash)
		*(pchCurSlash + 1) = '\0';

	strcpy(szBaseDir, szBuffer);
	cchLen = strlen(szBaseDir);

	// Remove the terminating '\\' or '/'.
	if (cchLen > 0)
	{
		if (szBaseDir[cchLen - 1] == '\\' || szBaseDir[cchLen - 1] == '/')
			szBaseDir[cchLen - 1] = '\0';
	}

	return szBaseDir;
}

/**
 * The launcher main function for Win32.
 *
 * @param hInstance A handle to the current instance of the application.
 * @param hPrevInstance A handle to the previous instance of the application.
 * @param lpCmdLine The command line for the application.
 * @param nShowCmd Controls how the window is to be shown.
 * @returns Zero on success; nonzero on error.
 */
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow)
{
	char *pszPath;

	char szModuleName[MAX_PATH];
	char szBuffer[4096];

	char *pszBaseDir;
	HINSTANCE launcher;

	// Get the current PATH. We're going to append the `bin	 directory to it.
	pszPath = getenv("PATH");

	if (!GetModuleFileName(hInstance, szModuleName, MAX_PATH))
	{
		MessageBoxA(NULL, "Failed calling GetModuleFileName", "Launcher Error",
		            MB_OK);

		return 0;
	}

	// Get the base directory of the executable.
	pszBaseDir = GetBaseDir(szModuleName);

	// Add the `bin` directory to the path.
	snprintf(szBuffer, sizeof(szBuffer),
	         "PATH=%s\\bin\\;%s",
	         pszBaseDir, pszPath);

	szBuffer[sizeof(szBuffer - 1)] = '\0';
	putenv(szBuffer);

	// Then, write the path of the launcher to szBuffer...
	snprintf(szBuffer, sizeof(szBuffer), "%s\\bin\\launcher.dll", pszBaseDir);
	szBuffer[(sizeof(szBuffer) - 1)] = '\0';

	// ...and load the library.
	launcher = LoadLibraryEx(szBuffer, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

	// Handle errors...
	if (!launcher)
	{
		char *pszErrorMsg;
		char szDiagnostic[1024];

		FormatMessage(
			// dwFlags
			(FORMAT_MESSAGE_ALLOCATE_BUFFER |
			 FORMAT_MESSAGE_FROM_SYSTEM |
			 FORMAT_MESSAGE_IGNORE_INSERTS),

			// lpSource
			NULL,

			// dwMessageId
			GetLastError(),

			// dwLanguageId
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),

			// lpBuffer
			(LPSTR)&pszErrorMsg,

			// nSize
			0,

			// Arguments
			NULL);

		snprintf(szDiagnostic, sizeof(szDiagnostic),
		         "Failed to locate the launcher DLL:\n\n%s",
		         pszErrorMsg);

		szDiagnostic[sizeof(szDiagnostic) - 1] = '\0';
		MessageBox(0, szDiagnostic, "Launcher Error", MB_OK);

		LocalFree(pszErrorMsg);
		return 0;
	}

	MainFn fnMain = (MainFn)GetProcAddress(launcher, "LauncherMain");
	return fnMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}
