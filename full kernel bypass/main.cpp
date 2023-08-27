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
		io::dbgprint( "cleaning status -> %i", cleaning::clean_traces( ) );
		io::dbgprint("tid -> %i", PsGetCurrentThreadId());

		// user extersize
		bool status = thread::unlink();
		io::dbgprint("unlinked thread -> %i", status);

		// change your process name here
		const char* p_name = "csgo.exe";
		io::dbgprint("process name -> %s", p_name);
		ULONG tPid = 0;
		PEPROCESS process_mem;

		 // scuff check to check if our peprocess is valid
		while (utils::process_by_name(p_name, &tPid, &process_mem) == 0)
		{
			io::dbgprint("waiting for -> %s", p_name);
			utils::sleep(2000);
		}
		io::dbgprint("found process -> %s with id -> %I64d", p_name, tPid);

		PVOID base_address = PsGetProcessSectionBaseAddress( process_mem ) ;
		io::dbgprint( "base address -> 0x%llx", base_address);

		io::dbgprint("sleeping for 30 seconds waiting for modules to load");
		utils::sleep(30000);

		KAPC_STATE apc;
		KeStackAttachProcess(process_mem, &apc); // TODO potential detection vector here (fix is using virtual to physical memory translation) thus removing the need for this conversion
		PVOID clientAddr = utils::get_module_entry(process_mem, L"engine.dll");
		KeUnstackDetachProcess(&apc);

		while (!clientAddr)
		{
			io::dbgprint("looking for client.dll module load");
			utils::sleep(3000);

			apc = { 0 };
			KeStackAttachProcess(process_mem, &apc);
			clientAddr = utils::get_module_entry(process_mem, L"engine.dll");
			KeUnstackDetachProcess(&apc);
		}
		io::dbgprint("Found client dll addr! -> %0x", clientAddr); 


		while (true) // main hack loop
		{
			DWORD client_state;
			memory::read_virtual_memory(tPid, process_mem, (char*)clientAddr + 0x59F19C, &client_state, sizeof(DWORD)); //clientState

			if (client_state) {
				DWORD local_player;
				memory::read_virtual_memory(tPid, process_mem, (PVOID)(client_state + 0x180), &local_player, sizeof(DWORD)); //localplatyer
				if (local_player) {
					DWORD health;
					memory::read_virtual_memory(tPid, process_mem, (PVOID)(local_player + 0x100), &health, sizeof(DWORD));//iHealth
					io::dbgprint(
						"[+] Found local player: 0x%X, health: %d\n",
						local_player,
						health
					);
				}
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

