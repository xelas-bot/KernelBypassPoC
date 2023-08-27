#pragma once
#include <ntdef.h>
#include <ntifs.h>
#include <ntddk.h>


namespace driver
{
	namespace utils
	{
		
		ULONG process_by_name(const char* process_name, ULONG* id, PEPROCESS* process );
		PVOID get_module_entry(PEPROCESS pe, LPCWSTR ModuleName);

		void sleep(int ms) { LARGE_INTEGER time;  time.QuadPart =- (ms) * 10 * 1000; KeDelayExecutionThread(KernelMode, TRUE, &time); }
	}
}