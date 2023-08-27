#pragma once
#include "utils.h"


typedef struct _PEB_LDR_DATA32
{
	ULONG Length;
	UCHAR Initialized;
	ULONG SsHandle;
	LIST_ENTRY32 InLoadOrderModuleList;
	LIST_ENTRY32 InMemoryOrderModuleList;
	LIST_ENTRY32 InInitializationOrderModuleList;
} PEB_LDR_DATA32, * PPEB_LDR_DATA32;

typedef struct _LDR_DATA_TABLE_ENTRY32
{
	LIST_ENTRY32 InLoadOrderLinks;
	LIST_ENTRY32 InMemoryOrderLinks;
	LIST_ENTRY32 InInitializationOrderLinks;
	ULONG DllBase;
	ULONG EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING32 FullDllName;
	UNICODE_STRING32 BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	LIST_ENTRY32 HashLinks;
	ULONG TimeDateStamp;
} LDR_DATA_TABLE_ENTRY32, * PLDR_DATA_TABLE_ENTRY32;

typedef struct _PEB32
{
	UCHAR InheritedAddressSpace;
	UCHAR ReadImageFileExecOptions;
	UCHAR BeingDebugged;
	UCHAR BitField;
	ULONG Mutant;
	ULONG ImageBaseAddress;
	ULONG Ldr;
	ULONG ProcessParameters;
	ULONG SubSystemData;
	ULONG ProcessHeap;
	ULONG FastPebLock;
	ULONG AtlThunkSListPtr;
	ULONG IFEOKey;
	ULONG CrossProcessFlags;
	ULONG UserSharedInfoPtr;
	ULONG SystemReserved;
	ULONG AtlThunkSListPtr32;
	ULONG ApiSetMap;
} PEB32, * PPEB32;

extern "C" {
	NTKERNELAPI
		PVOID
		NTAPI
		PsGetProcessWow64Process(_In_ PEPROCESS Process);
}

ULONG driver::utils::process_by_name(const char* process_name, ULONG* id, PEPROCESS* process)
{
	PLIST_ENTRY active_process_links;
	PEPROCESS start = IoGetCurrentProcess();
	PEPROCESS current_process = start;
	
	do
	{
		PKPROCESS kproc = (PKPROCESS)current_process;
		PDISPATCHER_HEADER header = (PDISPATCHER_HEADER)kproc;
		LPSTR current_process_name = (LPSTR)((PUCHAR)current_process + 0x5A8); // use vergillis to check struct stuff https://www.vergiliusproject.com/kernels/x64/Windows%2010%20%7C%202016/2110%2021H2%20(November%202021%20Update)/_EPROCESS

		if (header->SignalState == 0 && strcmp(current_process_name, process_name) == 0)
		{	
			*id = (ULONG)PsGetProcessId(current_process);
			*process = current_process;
			return *id;
		}

		active_process_links = (PLIST_ENTRY)((PUCHAR)current_process + 0x448);
		current_process = (PEPROCESS)(active_process_links->Flink);
		current_process = (PEPROCESS)((PUCHAR)current_process - 0x448);

	} while (start != current_process);

	return 0;
}


PVOID driver::utils::get_module_entry(PEPROCESS pe, LPCWSTR ModuleName) 
{
	if (!pe) { return 0; }
	__try
	{
		PPEB32 pPeb32 = (PPEB32)PsGetProcessWow64Process(pe);
		if (!pPeb32 || !pPeb32->Ldr) {
			return 0;
		}



		for (PLIST_ENTRY32 pListEntry = (PLIST_ENTRY32)((PPEB_LDR_DATA32)pPeb32->Ldr)
			->InLoadOrderModuleList.Flink;
			pListEntry != &((PPEB_LDR_DATA32)pPeb32->Ldr)->InLoadOrderModuleList;
			pListEntry = (PLIST_ENTRY32)pListEntry->Flink) {
			PLDR_DATA_TABLE_ENTRY32 pEntry =
				CONTAINING_RECORD(pListEntry, LDR_DATA_TABLE_ENTRY32, InLoadOrderLinks);

			if (wcscmp((PWCH)pEntry->BaseDllName.Buffer, ModuleName) == 0) {
				PVOID ModuleAddress = (PVOID)pEntry->DllBase;
				return ModuleAddress;
			}
		}

		return 0;
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
	}
	return 0;
}