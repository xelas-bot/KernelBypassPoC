#define WIN32_LEAN_AND_MEAN
#include <ntifs.h>
#include <ntimage.h>
#include <ntddk.h>
#include "defs.h"
#include "CallStack-Spoofer.h"
#include "io/io.h"
#include "utils/utils.h"
#include "memory/memory.h"
#include "thread/thread.h"
#include "cleaning/cleaning.h"
#define DWORD unsigned int
using namespace driver;


__declspec(align(16)) struct Color
{
	constexpr Color(const uint32 r, const uint32 g, const uint32 b, const uint32 a = 0x3f800000) noexcept :
		r(r), g(g), b(b), a(a) { }

	uint32 r, g, b, a;
};

namespace offsets
{
	constexpr DWORD dwEntityList = 0x4DFFF7C;
	constexpr DWORD m_bSpotted = 0x93D;
}

void driver_thread( void* context )
{
	// allow five seconds for driver to finish entry
	SPOOF_FUNC;
	io::dbgprint("sleeping for five seconds", PsGetCurrentThreadId());
	utils::sleep(5000);
	constexpr const auto color = Color{ 0x3f800000, 0x00000000, 0x3f800000 };

	__try {
		// debug text
		// io::dbgprint( "cleaning status -> %i", cleaning::clean_traces( ) );

		// user extersize
		io::dbgprint("unlinked thread -> %i", thread::unlink() );


		// change your process name here
		const char* p_name = "csgo.exe";
		NTSTATUS status = STATUS_SUCCESS;
		io::dbgprint("waiting for csgo.exe...");
		while (true)
		{
			status = memory::GetProcByName("csgo.exe", &memory::targetApplication, 0);
			if (NT_SUCCESS(status))
				break;

			utils::sleep(3000);
		}
		io::dbgprint("found process:%s ", p_name);

		io::dbgprint("getting pid...");
		HANDLE procId = PsGetProcessId(memory::targetApplication);

		if (!procId)
		{
			io::dbgprint("failed to find proc id");
			thread::link();
			PsTerminateSystemThread(0);
		}

		memory::pid = (ULONG)procId;
		io::dbgprint("got pid %i", memory::pid);

		io::dbgprint("sleeping for 30 seconds waiting for modules to load");
		utils::sleep(30000);

		int count = 0;
		while (!utils:: GetModuleBasex86(memory::targetApplication, L"serverbrowser.dll"))
		{
			if (count >= 30) //wait 30 sec then abort
			{
				io::dbgprint("failed to get serverbrowser dll\n");
				thread::link();
				return;
			}
			count++;
			utils::sleep(1000);
		}


		memory::clientBase = utils::GetModuleBasex86(memory::targetApplication, L"client.dll");
		if (!memory::clientBase)
		{
			io::dbgprint("failed to get clientBase");
			thread::link();
			return;
		}

		memory::engineBase = utils::GetModuleBasex86(memory::targetApplication, L"engine.dll");
		if (!memory::engineBase)
		{
			io::dbgprint("failed to get engineBase");
			thread::link();
			return;
		}

		io::dbgprint("All pointers found: clientbase: %x, enginebase: %x", memory::clientBase, memory::engineBase);

		PEPROCESS process = 0;
		while (true) {
			process = 0;
			NTSTATUS status = PsLookupProcessByProcessId((HANDLE)memory::pid, &process);
			if (!NT_SUCCESS(status) || !process )
			{
				io::dbgprint("process closed %i", memory::pid);
				thread::link();
				return;
			}
			
			DWORD localplayer = memory::ReadMemory<DWORD>(memory::clientBase + 0xDEA98C); // local player
			DWORD localTeam = memory::ReadMemory<DWORD>(localplayer + 0xF4);//fixed teamnum
			DWORD health = memory::ReadMemory<DWORD>(localplayer + 0x100); //ihealth
				
			io::dbgprint("Got localplayer params, team:%u, health:%u", localTeam, health);
			utils::sleep(1000);

			for (size_t i = 0; i < 64; ++i)
			{
				DWORD currEnt = memory::ReadMemory<DWORD>(memory::clientBase + 0x4DFFF7C + (i * 0x10)); //dw entity list
				if (!currEnt)
					continue;

				bool t = true;
				memory::WriteMemory<bool>(currEnt + 0x93D, &t); //spotted
			}

		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		io::dbgprint("exception in thread sleeper", GetExceptionCode());
		thread::link();
		PsTerminateSystemThread(GetExceptionCode());
		return;
	}

	//// sleep for 15 seconds to allow game to get started and prevent us from getting false info
	//utils::sleep(15000);

	//utils::process_by_name( process::process_name, &process::process );
	//io::dbgprint( "peprocess -> 0x%llx", process::process );

	//process::pid = reinterpret_cast< uint32 >( PsGetProcessId( process::process ) );
	//io::dbgprint("pid -> %i", process::pid);


	//// main loop
	//while ( true )
	//{
	//	
	//	//example read
	//	uint64 round_manager = memory::read< uint64 >( process::base_address + 0x77BF800 );
	//	uint32 encrypted_round_state = memory::read< uint32 >( round_manager + 0xC0 );
	//	uint32 decrypted_round_state = _rotl64( encrypted_round_state - 0x56, 0x1E );
	//	io::dbgprint( "round state ptr -> 0x%llx", decrypted_round_state );

	//	// example write
	//	memory::write< uint32 >( round_manager + 0xC0, 0x0 );

	//	// for testing
	//	if ( thread::terminate_thread ) 
	//	{
	//		io::dbgprint( "loops -> %i", thread::total_loops );
	//		utils::sleep( 5000 );
	//		thread::total_loops++;

	//		if ( thread::total_loops > thread::loops_before_end )
	//		{
	//			io::dbgprint( "terminating thread" );
	//			PsTerminateSystemThread( STATUS_SUCCESS );
	//		}
	//	}
	//}

	thread::link();
	PsTerminateSystemThread( STATUS_SUCCESS );
}

NTSTATUS DriverEntry( PDRIVER_OBJECT driver_object, PUNICODE_STRING registry_path ) {
	SPOOF_FUNC; // TODO THIS STILL DOESNT WORK SEE THE GITHUB README
	UNREFERENCED_PARAMETER( driver_object );
	UNREFERENCED_PARAMETER( registry_path );

	io::dbgprint("driver entry called.");

	__try {

		// change this per mapper; debug prints the entire mmu
		cleaning::debug = false;
		cleaning::driver_timestamp = 0x5284EAC3;
		cleaning::driver_name = RTL_CONSTANT_STRING(L"iqvw64e.sys");

		HANDLE thread_handle = nullptr;
		OBJECT_ATTRIBUTES object_attribues{ };
		InitializeObjectAttributes(&object_attribues, nullptr, OBJ_KERNEL_HANDLE, nullptr, nullptr);

		NTSTATUS status = PsCreateSystemThread(&thread_handle, 0, &object_attribues, nullptr, nullptr, reinterpret_cast<PKSTART_ROUTINE>(&driver_thread), nullptr);
		io::dbgprint("thread status -> 0x%llx", STATUS_SUCCESS);

		io::dbgprint("fininshed driver entry... HERRO...");
	}
	__except(EXCEPTION_EXECUTE_HANDLER) {
		io::dbgprint("exception in entry point", GetExceptionCode() );
	}
        
	return STATUS_SUCCESS;
}

