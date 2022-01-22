#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <iostream>
#include <string>

extern "C" __declspec(dllimport) void __stdcall ReplaceString(const char* srcString, const char* resString, DWORD pid);
typedef void __stdcall TReplaceString(const char*, const char*, DWORD);

void DynamicReplaceString(const char* input, const char* output, DWORD pid)
{
	// Get dll handle
	HMODULE hDllReplacer = LoadLibrary(L"DllReplacer.dll");

	if (hDllReplacer != NULL)
	{
		// Get function pointer
		TReplaceString* pReplaceString = (TReplaceString*)GetProcAddress(hDllReplacer, "_ReplaceString@12");

		// Call function
		if (pReplaceString != NULL)
		{
			pReplaceString(input, output, pid);
		}

		// Free dll handle
		FreeLibrary(hDllReplacer);
	}
}

void InjectLibrary(DWORD pid)
{
	// Open process instance
	HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION |
		PROCESS_CREATE_THREAD | PROCESS_CREATE_PROCESS, FALSE, pid);

	if (hProcess)
	{
		// Allocate memory for dll
		LPVOID baseAddress = VirtualAllocEx(hProcess, NULL, strlen("DllInjector.dll") + 1,
			MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

		if (baseAddress)
		{
			DWORD threadId;

			// Write dll in virtual memory and create new remote thread
			WriteProcessMemory(hProcess, baseAddress, "DllInjector.dll", strlen("DllInjector.dll") + 1, NULL);
			HANDLE hThread = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryA,
				(LPVOID)baseAddress, NULL, &threadId);

			// Wait thread detach
			if (hThread != NULL)
			{
				WaitForSingleObject(hThread, INFINITE);
			}	
		}

		// Free process handle
		CloseHandle(hProcess);
	}
}

int main()
{
	// Initialize process id
	DWORD pid = GetCurrentProcessId();

	// Initialize test strings
	char input1[] = "Test my own dll";
	char input2[] = "Test my own dll";
	const char result[] = "lld nwo ym tseT";

	// Output command
	printf("Commands:\n");
	printf("1 - Static import\n");
	printf("2 - Dynamic import\n");
	printf("3 - Inject library\n");
	printf("Enter command: ");

	// Input command
	int action;
	scanf("%i", &action);

	// Main part
	switch (action)
	{
	case 1:
		printf("\nInput strings: %s, %s\n", input1, input2);
		ReplaceString(input1, result, pid);
		printf("Result strings: %s, %s\n\n", input1, input2);

		break;
	case 2:
		printf("\nInput strings: %s, %s\n", input1, input2);
		DynamicReplaceString(input1, result, pid);
		printf("Result strings: %s, %s\n\n", input1, input2);

		break;
	case 3:
		printf("\nInput strings: %s, %s\n", input1, input2);
		InjectLibrary(pid);
		printf("Result strings: %s, %s\n\n", input1, input2);

		break;
	default:
		break;
	}

	system("pause");
	return 0;
}
