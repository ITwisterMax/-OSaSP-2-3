#include "pch.h"
#include <Windows.h>
#include <vector>

extern "C" void __declspec(dllexport) __stdcall ReplaceString(const char* input, const char* result, DWORD pid)
{
	// Open process instance
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, pid);

	if (hProcess)
	{
		// Sustem information
		SYSTEM_INFO sInfo;
		GetSystemInfo(&sInfo);

		// Memory information
		MEMORY_BASIC_INFORMATION mInfo;

		char* pointer = 0;
		std::vector<char> buffer;

		while (pointer < sInfo.lpMaximumApplicationAddress)
		{
			// Write information about current page in mInfo
			if (VirtualQueryEx(hProcess, pointer, &mInfo, sizeof(mInfo)) == sizeof(mInfo))
			{
				if (mInfo.AllocationProtect == PAGE_READWRITE && mInfo.State == MEM_COMMIT)
				{
					// Alignment address
					pointer = (char*)mInfo.BaseAddress;

					buffer.resize(mInfo.RegionSize);
					SIZE_T bytesRead;

					try
					{
						if (ReadProcessMemory(hProcess, pointer, &buffer[0], mInfo.RegionSize, &bytesRead))
						{
							for (size_t i = 0; i < (bytesRead - strlen(input)); ++i)
							{
								// Find necessary chars and rewrite them
								if (memcmp(input, &buffer[i], strlen(input)) == 0)
								{
									char* temp = (char*)pointer + i;

									for (int j = 0; j < strlen(result); j++)
									{
										temp[j] = result[j];
									}
									
									temp[strlen(result)] = 0;
								}
							}
						}
					}
					catch (std::bad_alloc& e)
					{

					}
				}
				pointer += mInfo.RegionSize;
			}
		}
	}
}