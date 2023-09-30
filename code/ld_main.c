#include <xi/xi.h>

#if XI_OS_WIN32
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)

#elif XI_OS_LINUX

int main(int argc, char **argv)

#endif

{
    xiGameCode code = { 0 };
    code.dynamic = true;

    int result = xie_run(&code);
    return result;
}

