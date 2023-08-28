#include <ntifs.h>

#pragma once
#define _USE_MATH_DEFINES
#include <math.h>

#if defined(__GNUC__)
typedef          long long ll;
typedef unsigned long long ull;
#define __int64 long long
#define __int32 int
#define __int16 short
#define __int8  char
#define MAKELL(num) num ## LL
#define FMT_64 "ll"
#elif defined(_MSC_VER)
typedef          __int64 ll;
typedef unsigned __int64 ull;
#define MAKELL(num) num ## i64
#define FMT_64 "I64"
#elif defined (__BORLANDC__)
typedef          __int64 ll;
typedef unsigned __int64 ull;
#define MAKELL(num) num ## i64
#define FMT_64 "L"
#else
#error "unknown compiler"
#endif
typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned short ushort;
//typedef unsigned long ulong;

typedef          char   int8;
typedef   signed char   sint8;
typedef unsigned char   uint8;
typedef          short  int16;
typedef   signed short  sint16;
typedef unsigned short  uint16;
typedef          int    int32;
typedef   signed int    sint32;
typedef unsigned int    uint32;
typedef ll              int64;
typedef ll              sint64;
typedef ull             uint64;

// Partially defined types:
#define _BYTE  uint8
#define _WORD  uint16
#define _DWORD uint32
#define _QWORD uint64
#if !defined(_MSC_VER)
#define _LONGLONG __int128
#endif


// Some convenience macros to make partial accesses nicer
// first unsigned macros:
//Already defined
//#define LOBYTE(x)   (*((_BYTE*)&(x)))   // low byte
//#define LOWORD(x)   (*((_WORD*)&(x)))   // low word
//#define LODWORD(x)  (*((_DWORD*)&(x)))  // low dword
//#define HIBYTE(x)   (*((_BYTE*)&(x)+1))
//#define HIWORD(x)   (*((_WORD*)&(x)+1))
#define HIDWORD(x)  (*((_DWORD*)&(x)+1))
#define BYTEn(x, n)   (*((_BYTE*)&(x)+n))
#define WORDn(x, n)   (*((_WORD*)&(x)+n))
#define BYTE1(x)   BYTEn(x,  1)         // byte 1 (counting from 0)
#define BYTE2(x)   BYTEn(x,  2)
#define BYTE3(x)   BYTEn(x,  3)
#define BYTE4(x)   BYTEn(x,  4)
#define BYTE5(x)   BYTEn(x,  5)
#define BYTE6(x)   BYTEn(x,  6)
#define BYTE7(x)   BYTEn(x,  7)
#define BYTE8(x)   BYTEn(x,  8)
#define BYTE9(x)   BYTEn(x,  9)
#define BYTE10(x)  BYTEn(x, 10)
#define BYTE11(x)  BYTEn(x, 11)
#define BYTE12(x)  BYTEn(x, 12)
#define BYTE13(x)  BYTEn(x, 13)
#define BYTE14(x)  BYTEn(x, 14)
#define BYTE15(x)  BYTEn(x, 15)
#define WORD1(x)   WORDn(x,  1)
#define WORD2(x)   WORDn(x,  2)         // third word of the object, unsigned
#define WORD3(x)   WORDn(x,  3)
#define WORD4(x)   WORDn(x,  4)
#define WORD5(x)   WORDn(x,  5)
#define WORD6(x)   WORDn(x,  6)
#define WORD7(x)   WORDn(x,  7)

// now signed macros (the same but with sign extension)
#define SLOBYTE(x)   (*((int8*)&(x)))
#define SLOWORD(x)   (*((int16*)&(x)))
#define SLODWORD(x)  (*((int32*)&(x)))
#define SHIBYTE(x)   (*((int8*)&(x)+1))
#define SHIWORD(x)   (*((int16*)&(x)+1))
#define SHIDWORD(x)  (*((int32*)&(x)+1))
#define SBYTEn(x, n)   (*((int8*)&(x)+n))
#define SWORDn(x, n)   (*((int16*)&(x)+n))
#define SBYTE1(x)   SBYTEn(x,  1)
#define SBYTE2(x)   SBYTEn(x,  2)
#define SBYTE3(x)   SBYTEn(x,  3)
#define SBYTE4(x)   SBYTEn(x,  4)
#define SBYTE5(x)   SBYTEn(x,  5)
#define SBYTE6(x)   SBYTEn(x,  6)
#define SBYTE7(x)   SBYTEn(x,  7)
#define SBYTE8(x)   SBYTEn(x,  8)
#define SBYTE9(x)   SBYTEn(x,  9)
#define SBYTE10(x)  SBYTEn(x, 10)
#define SBYTE11(x)  SBYTEn(x, 11)
#define SBYTE12(x)  SBYTEn(x, 12)
#define SBYTE13(x)  SBYTEn(x, 13)
#define SBYTE14(x)  SBYTEn(x, 14)
#define SBYTE15(x)  SBYTEn(x, 15)
#define SWORD1(x)   SWORDn(x,  1)
#define SWORD2(x)   SWORDn(x,  2)
#define SWORD3(x)   SWORDn(x,  3)
#define SWORD4(x)   SWORDn(x,  4)
#define SWORD5(x)   SWORDn(x,  5)
#define SWORD6(x)   SWORDn(x,  6)
#define SWORD7(x)   SWORDn(x,  7)

template<class T> T __ROL__(T value, int count)
{
    const uint nbits = sizeof(T) * 8;

    if (count > 0)
    {
        count %= nbits;
        T high = value >> (nbits - count);
        if (T(-1) < 0) // signed value
            high &= ~((T(-1) << count));
        value <<= count;
        value |= high;
    }
    else
    {
        count = -count % nbits;
        T low = value << (nbits - count);
        value >>= count;
        value |= low;
    }
    return value;
}

inline uint8  __ROL1__(uint8  value, int count) { return __ROL__((uint8)value, count); }
inline uint16 __ROL2__(uint16 value, int count) { return __ROL__((uint16)value, count); }
inline uint32 __ROL4__(uint32 value, int count) { return __ROL__((uint32)value, count); }
inline uint64 __ROL8__(uint64 value, int count) { return __ROL__((uint64)value, count); }
inline uint8  __ROR1__(uint8  value, int count) { return __ROL__((uint8)value, -count); }
inline uint16 __ROR2__(uint16 value, int count) { return __ROL__((uint16)value, -count); }
inline uint32 __ROR4__(uint32 value, int count) { return __ROL__((uint32)value, -count); }
inline uint64 __ROR8__(uint64 value, int count) { return __ROL__((uint64)value, -count); }

//Dumb glow decryption stuff
//https://www.codeproject.com/Articles/1274943/IEEE-754-Conversion

#define NTH_BIT(b, n) ((b >> n) & 0x1)

#define BYTE_TO_BIN(b)   (( b & 0x80 ) ) |\
            (( b & 0x40 ) ) |\
            (( b & 0x20 ) ) |\
            (( b & 0x10 ) ) |\
            (( b & 0x08 ) ) |\
            (( b & 0x04 ) ) |\
            (( b & 0x02 ) ) |\
            ( b & 0x01 )

#define MANTISSA_TO_BIN(b)  (( b & 0x400000 ) ) |\
             (( b & 0x200000 ) ) |\
             (( b & 0x100000 ) ) |\
             (( b &  0x80000 ) ) |\
             (( b &  0x40000 ) ) |\
             (( b &  0x20000 ) ) |\
             (( b &  0x10000 ) ) |\
             (( b &  0x8000 ) ) |\
             (( b &  0x4000 ) ) |\
             (( b &  0x2000 ) ) |\
             (( b &  0x1000 ) ) |\
             (( b &  0x800 ) ) |\
             (( b &  0x400 ) ) |\
             (( b &  0x200 ) ) |\
             (( b &  0x100 ) ) |\
             (( b &  0x80 ) ) |\
             (( b &  0x40 ) ) |\
             (( b &  0x20 ) ) |\
             (( b &  0x10 ) ) |\
             (( b &  0x08 ) ) |\
             (( b &  0x04 ) ) |\
             (( b &  0x02 ) ) |\
              ( b & 0x01 )

typedef union IEEE754
{
    struct
    {
        unsigned int mantissa : 23;
        unsigned int exponent : 8;
        unsigned int sign : 1;
    } raw;
    float f;
} IEEE754;

typedef struct _MM_UNLOADED_DRIVER
{
    UNICODE_STRING     Name;
    PVOID             ModuleStart;
    PVOID             ModuleEnd;
    ULONG64         UnloadTime;
} MM_UNLOADED_DRIVER, * PMM_UNLOADED_DRIVER;
typedef struct _PIDDBCACHE_ENTRY
{
    LIST_ENTRY        List;
    UNICODE_STRING    DriverName;
    ULONG            TimeDateStamp;
    NTSTATUS        LoadStatus;
    char            _0x0028[16];
} PIDDBCACHE_ENTRY, * PPIDDBCACHE_ENTRY;
typedef struct _SYSTEM_MODULEE
{
    ULONG_PTR Reserved[2];
    PVOID     Base;
    ULONG     Size;
    ULONG     Flags;
    USHORT    Index;
    USHORT    Unknown;
    USHORT    LoadCount;
    USHORT    ModuleNameOffset;
    CHAR      ImageName[256];
} SYSTEM_MODULEE, * PSYSTEM_MODULEE;
typedef struct _SYSTEM_MODULE_INFORMATIONN
{
    ULONG_PTR     ModuleCount;
    SYSTEM_MODULEE Modules[1];
} SYSTEM_MODULE_INFORMATIONN, * PSYSTEM_MODULE_INFORMATIONN;


//0x1 bytes (sizeof)
union _KWAIT_STATUS_REGISTER
{
    UCHAR Flags;                                                            //0x0
    UCHAR State : 3;                                                          //0x0
    UCHAR Affinity : 1;                                                       //0x0
    UCHAR Priority : 1;                                                       //0x0
    UCHAR Apc : 1;                                                            //0x0
    UCHAR UserApc : 1;                                                        //0x0
    UCHAR Alert : 1;                                                          //0x0
};

//0x430 bytes (sizeof)
struct MY_KTHREAD
{
    struct _DISPATCHER_HEADER Header;                                       //0x0
    VOID* SListFaultAddress;                                                //0x18
    ULONGLONG QuantumTarget;                                                //0x20
    VOID* InitialStack;                                                     //0x28
    VOID* volatile StackLimit;                                              //0x30
    VOID* StackBase;                                                        //0x38
    ULONGLONG ThreadLock;                                                   //0x40
    volatile ULONGLONG CycleTime;                                           //0x48
    ULONG CurrentRunTime;                                                   //0x50
    ULONG ExpectedRunTime;                                                  //0x54
    VOID* KernelStack;                                                      //0x58
    struct _XSAVE_FORMAT* StateSaveArea;                                    //0x60
    struct _KSCHEDULING_GROUP* volatile SchedulingGroup;                    //0x68
    union _KWAIT_STATUS_REGISTER WaitRegister;                              //0x70
    volatile UCHAR Running;                                                 //0x71
    UCHAR Alerted[2];                                                       //0x72
    union
    {
        struct
        {
            ULONG AutoBoostActive : 1;                                        //0x74
            ULONG ReadyTransition : 1;                                        //0x74
            ULONG WaitNext : 1;                                               //0x74
            ULONG SystemAffinityActive : 1;                                   //0x74
            ULONG Alertable : 1;                                              //0x74
            ULONG UserStackWalkActive : 1;                                    //0x74
            ULONG ApcInterruptRequest : 1;                                    //0x74
            ULONG QuantumEndMigrate : 1;                                      //0x74
            ULONG UmsDirectedSwitchEnable : 1;                                //0x74
            ULONG TimerActive : 1;                                            //0x74
            ULONG SystemThread : 1;                                           //0x74
            ULONG ProcessDetachActive : 1;                                    //0x74
            ULONG CalloutActive : 1;                                          //0x74
            ULONG ScbReadyQueue : 1;                                          //0x74
            ULONG ApcQueueable : 1;                                           //0x74
            ULONG ReservedStackInUse : 1;                                     //0x74
            ULONG UmsPerformingSyscall : 1;                                   //0x74
            ULONG TimerSuspended : 1;                                         //0x74
            ULONG SuspendedWaitMode : 1;                                      //0x74
            ULONG SuspendSchedulerApcWait : 1;                                //0x74
            ULONG CetUserShadowStack : 1;                                     //0x74
            ULONG BypassProcessFreeze : 1;                                    //0x74
            ULONG Reserved : 10;                                              //0x74
        };
        LONG MiscFlags;                                                     //0x74
    };
    union
    {
        struct
        {
            ULONG ThreadFlagsSpare : 2;                                       //0x78
            ULONG AutoAlignment : 1;                                          //0x78
            ULONG DisableBoost : 1;                                           //0x78
            ULONG AlertedByThreadId : 1;                                      //0x78
            ULONG QuantumDonation : 1;                                        //0x78
            ULONG EnableStackSwap : 1;                                        //0x78
            ULONG GuiThread : 1;                                              //0x78
            ULONG DisableQuantum : 1;                                         //0x78
            ULONG ChargeOnlySchedulingGroup : 1;                              //0x78
            ULONG DeferPreemption : 1;                                        //0x78
            ULONG QueueDeferPreemption : 1;                                   //0x78
            ULONG ForceDeferSchedule : 1;                                     //0x78
            ULONG SharedReadyQueueAffinity : 1;                               //0x78
            ULONG FreezeCount : 1;                                            //0x78
            ULONG TerminationApcRequest : 1;                                  //0x78
            ULONG AutoBoostEntriesExhausted : 1;                              //0x78
            ULONG KernelStackResident : 1;                                    //0x78
            ULONG TerminateRequestReason : 2;                                 //0x78
            ULONG ProcessStackCountDecremented : 1;                           //0x78
            ULONG RestrictedGuiThread : 1;                                    //0x78
            ULONG VpBackingThread : 1;                                        //0x78
            ULONG ThreadFlagsSpare2 : 1;                                      //0x78
            ULONG EtwStackTraceApcInserted : 8;                               //0x78
        };
        volatile LONG ThreadFlags;                                          //0x78
    };
    volatile UCHAR Tag;                                                     //0x7c
    UCHAR SystemHeteroCpuPolicy;                                            //0x7d
    UCHAR UserHeteroCpuPolicy : 7;                                            //0x7e
    UCHAR ExplicitSystemHeteroCpuPolicy : 1;                                  //0x7e
    union
    {
        struct
        {
            UCHAR RunningNonRetpolineCode : 1;                                //0x7f
            UCHAR SpecCtrlSpare : 7;                                          //0x7f
        };
        UCHAR SpecCtrl;                                                     //0x7f
    };
    ULONG SystemCallNumber;                                                 //0x80
    ULONG ReadyTime;                                                        //0x84
    VOID* FirstArgument;                                                    //0x88
    struct _KTRAP_FRAME* TrapFrame;                                         //0x90
    union
    {
        struct _KAPC_STATE ApcState;                                        //0x98
        struct
        {
            UCHAR ApcStateFill[43];                                         //0x98
            CHAR Priority;                                                  //0xc3
            ULONG UserIdealProcessor;                                       //0xc4
        };
    };
    volatile LONGLONG WaitStatus;                                           //0xc8
    struct _KWAIT_BLOCK* WaitBlockList;                                     //0xd0
    union
    {
        struct _LIST_ENTRY WaitListEntry;                                   //0xd8
        struct _SINGLE_LIST_ENTRY SwapListEntry;                            //0xd8
    };
    struct _DISPATCHER_HEADER* volatile Queue;                              //0xe8
    VOID* Teb;                                                              //0xf0
    ULONGLONG RelativeTimerBias;                                            //0xf8
    struct _KTIMER Timer;                                                   //0x100
    union
    {
        struct _KWAIT_BLOCK WaitBlock[4];                                   //0x140
        struct
        {
            UCHAR WaitBlockFill4[20];                                       //0x140
            ULONG ContextSwitches;                                          //0x154
        };
        struct
        {
            UCHAR WaitBlockFill5[68];                                       //0x140
            volatile UCHAR State;                                           //0x184
            CHAR Spare13;                                                   //0x185
            UCHAR WaitIrql;                                                 //0x186
            CHAR WaitMode;                                                  //0x187
        };
        struct
        {
            UCHAR WaitBlockFill6[116];                                      //0x140
            ULONG WaitTime;                                                 //0x1b4
        };
        struct
        {
            UCHAR WaitBlockFill7[164];                                      //0x140
            union
            {
                struct
                {
                    SHORT KernelApcDisable;                                 //0x1e4
                    SHORT SpecialApcDisable;                                //0x1e6
                };
                ULONG CombinedApcDisable;                                   //0x1e4
            };
        };
        struct
        {
            UCHAR WaitBlockFill8[40];                                       //0x140
            struct _KTHREAD_COUNTERS* ThreadCounters;                       //0x168
        };
        struct
        {
            UCHAR WaitBlockFill9[88];                                       //0x140
            struct _XSTATE_SAVE* XStateSave;                                //0x198
        };
        struct
        {
            UCHAR WaitBlockFill10[136];                                     //0x140
            VOID* volatile Win32Thread;                                     //0x1c8
        };
        struct
        {
            UCHAR WaitBlockFill11[176];                                     //0x140
            struct _UMS_CONTROL_BLOCK* Ucb;                                 //0x1f0
            struct _KUMS_CONTEXT_HEADER* volatile Uch;                      //0x1f8
        };
    };
    union
    {
        volatile LONG ThreadFlags2;                                         //0x200
        struct
        {
            ULONG BamQosLevel : 8;                                            //0x200
            ULONG ThreadFlags2Reserved : 24;                                  //0x200
        };
    };
    ULONG Spare21;                                                          //0x204
    struct _LIST_ENTRY QueueListEntry;                                      //0x208
    union
    {
        volatile ULONG NextProcessor;                                       //0x218
        struct
        {
            ULONG NextProcessorNumber : 31;                                   //0x218
            ULONG SharedReadyQueue : 1;                                       //0x218
        };
    };
    LONG QueuePriority;                                                     //0x21c
    struct _KPROCESS* Process;                                              //0x220
    union
    {
        struct _GROUP_AFFINITY UserAffinity;                                //0x228
        struct
        {
            UCHAR UserAffinityFill[10];                                     //0x228
            CHAR PreviousMode;                                              //0x232
            CHAR BasePriority;                                              //0x233
            union
            {
                CHAR PriorityDecrement;                                     //0x234
                struct
                {
                    UCHAR ForegroundBoost : 4;                                //0x234
                    UCHAR UnusualBoost : 4;                                   //0x234
                };
            };
            UCHAR Preempted;                                                //0x235
            UCHAR AdjustReason;                                             //0x236
            CHAR AdjustIncrement;                                           //0x237
        };
    };
    ULONGLONG AffinityVersion;                                              //0x238
    union
    {
        struct _GROUP_AFFINITY Affinity;                                    //0x240
        struct
        {
            UCHAR AffinityFill[10];                                         //0x240
            UCHAR ApcStateIndex;                                            //0x24a
            UCHAR WaitBlockCount;                                           //0x24b
            ULONG IdealProcessor;                                           //0x24c
        };
    };
    ULONGLONG NpxState;                                                     //0x250
    union
    {
        struct _KAPC_STATE SavedApcState;                                   //0x258
        struct
        {
            UCHAR SavedApcStateFill[43];                                    //0x258
            UCHAR WaitReason;                                               //0x283
            CHAR SuspendCount;                                              //0x284
            CHAR Saturation;                                                //0x285
            USHORT SListFaultCount;                                         //0x286
        };
    };
    union
    {
        struct _KAPC SchedulerApc;                                          //0x288
        struct
        {
            UCHAR SchedulerApcFill0[1];                                     //0x288
            UCHAR ResourceIndex;                                            //0x289
        };
        struct
        {
            UCHAR SchedulerApcFill1[3];                                     //0x288
            UCHAR QuantumReset;                                             //0x28b
        };
        struct
        {
            UCHAR SchedulerApcFill2[4];                                     //0x288
            ULONG KernelTime;                                               //0x28c
        };
        struct
        {
            UCHAR SchedulerApcFill3[64];                                    //0x288
            struct _KPRCB* volatile WaitPrcb;                               //0x2c8
        };
        struct
        {
            UCHAR SchedulerApcFill4[72];                                    //0x288
            VOID* LegoData;                                                 //0x2d0
        };
        struct
        {
            UCHAR SchedulerApcFill5[83];                                    //0x288
            UCHAR CallbackNestingLevel;                                     //0x2db
            ULONG UserTime;                                                 //0x2dc
        };
    };
    struct _KEVENT SuspendEvent;                                            //0x2e0
    struct _LIST_ENTRY ThreadListEntry;                                     //0x2f8
    struct _LIST_ENTRY MutantListHead;                                      //0x308
    UCHAR AbEntrySummary;                                                   //0x318
    UCHAR AbWaitEntryCount;                                                 //0x319
    UCHAR AbAllocationRegionCount;                                          //0x31a
    CHAR SystemPriority;                                                    //0x31b
    ULONG SecureThreadCookie;                                               //0x31c
    struct _KLOCK_ENTRY* LockEntries;                                       //0x320
    struct _SINGLE_LIST_ENTRY PropagateBoostsEntry;                         //0x328
    struct _SINGLE_LIST_ENTRY IoSelfBoostsEntry;                            //0x330
    UCHAR PriorityFloorCounts[16];                                          //0x338
    UCHAR PriorityFloorCountsReserved[16];                                  //0x348
    ULONG PriorityFloorSummary;                                             //0x358
    volatile LONG AbCompletedIoBoostCount;                                  //0x35c
    volatile LONG AbCompletedIoQoSBoostCount;                               //0x360
    volatile SHORT KeReferenceCount;                                        //0x364
    UCHAR AbOrphanedEntrySummary;                                           //0x366
    UCHAR AbOwnedEntryCount;                                                //0x367
    ULONG ForegroundLossTime;                                               //0x368
    union
    {
        struct _LIST_ENTRY GlobalForegroundListEntry;                       //0x370
        struct
        {
            struct _SINGLE_LIST_ENTRY ForegroundDpcStackListEntry;          //0x370
            ULONGLONG InGlobalForegroundList;                               //0x378
        };
    };
    LONGLONG ReadOperationCount;                                            //0x380
    LONGLONG WriteOperationCount;                                           //0x388
    LONGLONG OtherOperationCount;                                           //0x390
    LONGLONG ReadTransferCount;                                             //0x398
    LONGLONG WriteTransferCount;                                            //0x3a0
    LONGLONG OtherTransferCount;                                            //0x3a8
    struct _KSCB* QueuedScb;                                                //0x3b0
    volatile ULONG ThreadTimerDelay;                                        //0x3b8
    union
    {
        volatile LONG ThreadFlags3;                                         //0x3bc
        struct
        {
            ULONG ThreadFlags3Reserved : 8;                                   //0x3bc
            ULONG PpmPolicy : 2;                                              //0x3bc
            ULONG ThreadFlags3Reserved2 : 22;                                 //0x3bc
        };
    };
    ULONGLONG TracingPrivate[1];                                            //0x3c0
    VOID* SchedulerAssist;                                                  //0x3c8
    VOID* volatile AbWaitObject;                                            //0x3d0
    ULONG ReservedPreviousReadyTimeValue;                                   //0x3d8
    ULONGLONG KernelWaitTime;                                               //0x3e0
    ULONGLONG UserWaitTime;                                                 //0x3e8
    union
    {
        struct _LIST_ENTRY GlobalUpdateVpThreadPriorityListEntry;           //0x3f0
        struct
        {
            struct _SINGLE_LIST_ENTRY UpdateVpThreadPriorityDpcStackListEntry; //0x3f0
            ULONGLONG InGlobalUpdateVpThreadPriorityList;                   //0x3f8
        };
    };
    LONG SchedulerAssistPriorityFloor;                                      //0x400
    ULONG Spare28;                                                          //0x404
    ULONGLONG EndPadding[5];                                                //0x408
};


inline auto dereference(uintptr_t address, unsigned int offset) -> uintptr_t
{
    if (address == 0)
        return 0;

    return address + (int)((*(int*)(address + offset) + offset) + sizeof(int));
}
inline auto relative(uintptr_t address, unsigned int size) -> PVOID
{
    if (address == 0)
        return 0;

    return ((PVOID)((unsigned char*)(address)+*(int*)((unsigned char*)(address)+((size)-(INT)sizeof(INT))) + (size)));
}
inline auto compare_data(const unsigned char* pData, const unsigned char* bMask, const char* szMask) -> bool
{
    for (; *szMask; ++szMask, ++pData, ++bMask)
        if (*szMask == 'x' && *pData != *bMask)
            return 0;

    return (*szMask) == 0;
}
inline auto find_pattern2(UINT64 dwAddress, UINT64 dwLen, unsigned char* bMask, const char* szMask) -> ULONGLONG
{
    for (ULONGLONG i = 0; i < dwLen; i++)
        if (compare_data((unsigned char*)(dwAddress + i), bMask, szMask))
            return (ULONGLONG)(dwAddress + i);

    return 0;
}
template <typename t = void*>
inline auto find_pattern(void* start, size_t length, const char* pattern, const char* mask) -> t
{
    const auto data = static_cast<const char*>(start);
    const auto pattern_length = strlen(mask);

    for (size_t i = 0; i <= length - pattern_length; i++)
    {
        bool accumulative_found = true;

        for (size_t j = 0; j < pattern_length; j++)
        {
            if (!MmIsAddressValid(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(data) + i + j)))
            {
                accumulative_found = false;
                break;
            }

            if (data[i + j] != pattern[j] && mask[j] != '?')
            {
                accumulative_found = false;
                break;
            }
        }

        if (accumulative_found)
        {
            return (t)(reinterpret_cast<uintptr_t>(data) + i);
        }
    }

    return (t)nullptr;
}