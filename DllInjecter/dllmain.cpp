#include "pch.h"
#include "../DllReplacer/replacer.cpp"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    // Initialize process id
    DWORD pid = GetCurrentProcessId();

    // Initialize test strings
    char input1[] = "Test my own dll";
    char input2[] = "Test my own dll";
    const char result[] = "lld nwo ym tseT";

    // Main part
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        ReplaceString(input1, result, pid);
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

