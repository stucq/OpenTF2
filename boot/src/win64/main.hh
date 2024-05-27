#ifndef BOOT_WIN32_MAIN_HH
#define BOOT_WIN32_MAIN_HH

#include <windows.h>

/**
 * The signature of the main function.
 *
 * This is defined as a unique type as it differs from platform to platform.
 */
typedef int (*MainFn)(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                      LPSTR lpCmdLine, int nCmdShow);

#endif /* BOOT_WIN32_MAIN_HH */
