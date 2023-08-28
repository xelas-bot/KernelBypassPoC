#pragma once
#include "Nt.h"
#include "io.h"

namespace driver
{
	namespace memory
	{
		PEPROCESS targetApplication = nullptr;
		ULONG pid = NULL;
		UINT64 clientBase = NULL;
		UINT64 engineBase = NULL;

		template<typename t>
		t ReadMemory(UINT64 addr)
		{
			t buffer;

			SIZE_T copiedBytes = 0;
			MmCopyVirtualMemory(targetApplication, (PVOID)addr, PsGetCurrentProcess(), &buffer, sizeof(t), KernelMode, &copiedBytes);

			return buffer;
		}

		template<typename t>
		SIZE_T WriteMemory(UINT64 addr, t* buffer)
		{

			SIZE_T copiedBytes = 0;
			MmCopyVirtualMemory(PsGetCurrentProcess(), buffer, targetApplication , (PVOID)addr, sizeof(t), KernelMode, &copiedBytes);

			return copiedBytes;
		}


		template<typename t>
		NTSTATUS ReadVirtual(PEPROCESS process, uintptr_t addr, t* buffer)
		{
			if (!process || !addr || !buffer)
				return STATUS_INVALID_PARAMETER;

			SIZE_T copiedBytes = 0;
			return MmCopyVirtualMemory(process, (PVOID)addr, PsGetCurrentProcess(), buffer, sizeof(t), KernelMode, &copiedBytes);
		}

		template<typename t>
		NTSTATUS WriteVirtual(PEPROCESS process, uintptr_t addr, t* buffer)
		{
			if (!process || !addr || !buffer)
				return STATUS_INVALID_PARAMETER;

			SIZE_T copiedBytes = 0;
			return MmCopyVirtualMemory(PsGetCurrentProcess(), buffer, process, (PVOID)addr, sizeof(t), KernelMode, &copiedBytes);
		}

		NTSTATUS GetProcessBaseAddress(int pid, ULONG64* baseAddr)
		{
			PEPROCESS eproc = NULL;
			if (pid == 0)
				return STATUS_INVALID_PARAMETER;

			NTSTATUS status = PsLookupProcessByProcessId((HANDLE)pid, &eproc);
			if (!NT_SUCCESS(status))
				return status;

			PVOID base = PsGetProcessSectionBaseAddress(eproc);

			ObDereferenceObject(eproc);
			*baseAddr = (ULONG64)base;

			return STATUS_SUCCESS;
		}

		inline ULONG RandomNumber()
		{
			ULONG64 tickCount;
			KeQueryTickCount(&tickCount);

			return RtlRandomEx((PULONG)&tickCount);
		}

		void WriteRandom(ULONG64 addr, ULONG size)
		{
			for (size_t i = 0; i < size; i++)
			{
				*(char*)(addr + i) = RandomNumber() % 255;
			}
		}

		PVOID AllocatePoolMemory(ULONG size)
		{
			return ExAllocatePool(NonPagedPool, size);
		}

		void FreePoolMemory(PVOID base, ULONG size)
		{
			for (size_t i = 0; i < size; i++)
			{
				*(char*)((UINT64)base + i) = RandomNumber() % 255;
			}
			ExFreePoolWithTag(base, 0);
		}

		PVOID QuerySystemInformation(SYSTEM_INFORMATION_CLASS SystemInfoClass, ULONG* size)
		{
			int currAttempt = 0;
			int maxAttempt = 20;


		QueryTry:
			if (currAttempt >= maxAttempt)
				return 0;

			currAttempt++;
			ULONG neededSize = 0;
			ZwQuerySystemInformation(SystemInfoClass, NULL, neededSize, &neededSize);
			if (!neededSize)
				goto QueryTry;

			ULONG allocationSize = neededSize;
			PVOID informationBuffer = AllocatePoolMemory(allocationSize);
			if (!informationBuffer)
				goto QueryTry;

			NTSTATUS status = ZwQuerySystemInformation(SystemInfoClass, informationBuffer, neededSize, &neededSize);
			if (!NT_SUCCESS(status))
			{
				FreePoolMemory(informationBuffer, allocationSize);
				goto QueryTry;
			}

			*size = allocationSize;
			return informationBuffer;
		}


		NTSTATUS GetProcByName(const char* name, PEPROCESS* process, int iteration)
		{
			ANSI_STRING nameAnsi;
			RtlInitAnsiString(&nameAnsi, name);

			UNICODE_STRING nameUnicode;
			NTSTATUS status = RtlAnsiStringToUnicodeString(&nameUnicode, &nameAnsi, true);
			if (!NT_SUCCESS(status))
			{
				io::dbgprint("Failed to translate cstr to unicode str %x\n", status);
				return status;
			}

			ULONG size = 0;
			PSYSTEM_PROCESS_INFORMATION procInfo = (PSYSTEM_PROCESS_INFORMATION)QuerySystemInformation(SystemProcessInformation, &size);
			if (!procInfo || !size)
			{
				io::dbgprint("Failed to query process information\n");
				return status;
			}

			PSYSTEM_PROCESS_INFORMATION currEntry = procInfo;

			int currIteration = 0;

			while (true)
			{
				if (!RtlCompareUnicodeString(&nameUnicode, &currEntry->ImageName, true))
				{
					if (currIteration != iteration)
					{
						currIteration++;

						if (!currEntry->NextEntryOffset)
							break;

						currEntry = (PSYSTEM_PROCESS_INFORMATION)((char*)currEntry + currEntry->NextEntryOffset);
						continue;
					}

					if (0 >= currEntry->NumberOfThreads)
					{
						io::dbgprint("process has no active threads\n");
						if (!currEntry->NextEntryOffset)
							break;

						currEntry = (PSYSTEM_PROCESS_INFORMATION)((char*)currEntry + currEntry->NextEntryOffset);
						continue;
					}

					ULONGLONG pid = currEntry->UniqueProcessId;
					PEPROCESS foundProcess = 0;
					status = PsLookupProcessByProcessId((HANDLE)pid, &foundProcess);
					if (!NT_SUCCESS(status))
					{
						io::dbgprint("failed to lookup process by id %x\n", status);
						if (!currEntry->NextEntryOffset)
							break;

						currEntry = (PSYSTEM_PROCESS_INFORMATION)((char*)currEntry + currEntry->NextEntryOffset);
						continue;
					}

					FreePoolMemory(procInfo, size);
					io::dbgprint("found process with pid %i\n", pid);
					*process = foundProcess;
					return STATUS_SUCCESS;
				}

				if (!currEntry->NextEntryOffset)
					break;

				currEntry = (PSYSTEM_PROCESS_INFORMATION)((char*)currEntry + currEntry->NextEntryOffset);
			}

			FreePoolMemory(procInfo, size);
			return STATUS_NOT_FOUND;
		}


		UINT64 GetKernelModuleBase(const char* name)
		{
			ULONG size = 0;
			PSYSTEM_MODULE_INFORMATION moduleInformation = (PSYSTEM_MODULE_INFORMATION)QuerySystemInformation(SystemModuleInformation, &size);

			if (!moduleInformation || !size)
				return 0;

			for (size_t i = 0; i < moduleInformation->Count; i++)
			{
				char* fileName = (char*)moduleInformation->Module[i].FullPathName + moduleInformation->Module[i].OffsetToFileName;
				if (!strcmp(fileName, name))
				{
					UINT64 imageBase = (UINT64)moduleInformation->Module[i].ImageBase;
					FreePoolMemory(moduleInformation, size);
					return imageBase;
				}
			}

			FreePoolMemory(moduleInformation, size);
		}

		bool IsProcessName(const char* name, PEPROCESS process)
		{
			char procName[15];
			memcpy(&procName, PsGetProcessImageFileName(process), sizeof(procName));
			if (!strcmp((char*)&procName, name))
				return true;

			else
				return false;
		}

		bool IsProcessName(const char* name, int pid)
		{
			PEPROCESS process = 0;
			NTSTATUS status = PsLookupProcessByProcessId((HANDLE)pid, &process);
			if (!NT_SUCCESS(status))
			{
				io::dbgprint("Failed to get process with pid %i", pid);
				return false;
			}

			if (!process)
			{
				io::dbgprint("Failed to get process");
				return false;
			}

			char procName[15];
			memcpy(&procName, PsGetProcessImageFileName(process), sizeof(procName));

			ObfDereferenceObject(process);


			if (!strcmp((char*)&procName, name))
				return true;

			else
			{
				//Print("proc name %s\n", procName);
				return false;
			}
		}
	};
}