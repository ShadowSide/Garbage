#include <stdafx.h>
// CNC.cpp
//-------------------------------------------------------------------------------

#include "TargetWrapper.h"

#include <rtk32.h>
#include <Rttarget.h>

//#include <crtdbg.h>
#include <stdlib.h>

#include "global_vars.h"
#include "SwopCells.h"

#include "main.h"
#include "dialog.h"
#include "ext_global.h"
#include "fixup_params.h"

#include "drv.h"
#include "com_const.h"
#include "PC1601.h"
#include "PC8604.h"
#include "com_drv_pult.h"

#include "portManagement.h"
#include "GUIInitializer.h"
#include "ProjectConfiguration.h"
#include "NetworkingInitialize.h"
#include "FileTransfer.Initializer.h"
#include "CrtHappy/CrtHappy.h"
#include "Tests.h"

#include <iostream>

#pragma data_seg("Ver")
	char versionCNC[10]="1.4.3064";
	char dateCNC[]=__DATE__;
	char timeCNC[]=__TIME__;
	char reservedBin[12]="org"; // резерв для выравнивания CRC секции
	// 0x3030
	volatile WORD crcCurrentU1=0xFFFF;
	volatile WORD crcCurrentU2=0xFFFF;
#pragma data_seg() 

//-------------------------------------------------------------------------------
const char CENDCOND = 'a';

__declspec(naked) void __cdecl NMIHandler(void)     // low level int handler to setup
{                                  // interrupt stack frame
   _asm {
      push  eax            ; save all registers which may be changed by C/C++ code
      push  edx
      mov   dx, 0x666         ; data selector comes right after
	  in al, dx
	  mov dx, REG_BUS
	  in al, dx
	  xor al, MASK_BUS_SVGREEN	// мигаем зеленой лампочкой
	  and al, ~MASK_BUS_SVRED	// гасим желтую
	  out dx, al
      pop   edx
      pop   eax
      iretd                ; interrupt return		   
   }
}

__declspec(naked) void __cdecl ResetNMI(void)     // low level int handler to setup
{                                  // interrupt stack frame
   _asm {
      push  eax            ; save all registers which may be changed by C/C++ code
      push  edx
      // mov   dx, 0x666         ; data selector comes right after
	  // in al, dx
	  mov dx, REG_BUS
	  in al, dx
	  or al, MASK_BUS_SVGREEN	// зажигаем и зеленую и желтую лампочки
	  or al, MASK_BUS_SVRED	
	  out dx, al
      pop   edx
      pop   eax
      iretd                ; interrupt return		   
   }
}

// программный перезапуск
void restart(TElanSC520)
{
	RTOut(REG_RESTART, MASK_R_PRG_RES); 
}
// программный перезапуск из другого банка
void restartWD(TElanSC520)
{
	// запрещаем пррывания от WD
	RTOut(REG_WD, RTIn(REG_WD) & ~MASK_WD_EN_IRQ); // бит разрешения выдачи IRQ от WD 
	RTSetIntVector(2, ResetNMI);
	RTKDelay(1000);
	// разрешаем пррывания от WD
	RTOut(REG_WD, RTIn(REG_WD) | MASK_WD_EN_IRQ); // бит разрешения выдачи IRQ от WD 
	RTKDelay(1000);
}

// программный перезапуск
void restart(TStandartPC)
{
	RTKFatalError("Manual PC reset.");
	//RTKExit(0);
}
// программный перезапуск из другого банка
void restartWD(TStandartPC)
{
	restart();
}

/*--------------------------------------------------------------------------*/
/* RTTarget-32 init function executed before the startup code               */
extern "C" {
#ifdef _MSC_VER
	__declspec(dllexport) void __cdecl InitRTOS32KernelBeforeStartApplication(void)
#else
	void __export __cdecl InitRTOS32KernelBeforeStartApplication(void)
#endif
	{
		RTSetFlags(RT_MM_VIRTUAL, 1);
#ifdef NOT_SEND_DEBUG_OUTPUT_TO_DEBUG_CONSOLE
		RTSetFlags(RT_DBG_OUT_NONE, 1);
#endif
		RTCMOSExtendHeap();            // get as much memory as we can
		RTCMOSSetSystemTime();         // get the right date and time
		// RTEmuInit();                   // only if you need the 387 emulator

		// ...............Here init Kernel...................

	};
};

void InitProtection(PULT_PLATFORM platform=PULT_PLATFORM());
void InitProtection(TStandartPC)
{
};
void InitProtection(TElanSC520)
{
	//	регион 7 - ПЗУ пользователя, R/W
	//  0x00000000  запись разрешена                      бит=0
	//  0x04000000  защита от записи (запись блокируется) бит=1
	//	разрешить запись в Flash
	MMCR_DWORD(PAR7) &= ~MASK_ACCESS_MEMORY_WRITE;  // сбросить бит в 0 в регионе 7
	RTOut(REG_ACS_FLASH, RTIn(REG_ACS_FLASH)|MASK_FLASH_EN_WR);// установить бит 5 порта 0x203
	RTSetIntVector(2, NMIHandler);//Установка обработчика IRQ от WD
	RTOut(REG_WD, RTIn(REG_WD) | MASK_WD_EN_IRQ); // бит разрешения выдачи IRQ от WD 

};
//-------------------------------------------------------------------------------
// Сообщения в консоль
void DebugConsoleMessage(PChar, TDebugInfo<OFF>) {}
void DebugConsoleMessage(PChar Message, TDebugInfo<ON>)
{
	puts(Message);
}
void DebugConsoleMessage(PChar Message)
{
	DebugConsoleMessage(Message, TConfigurationTraits<SCurrentConfig>::DebugInfo());
};

//-------------------------------------------------------------------------------

void no_memory_handler () {
	//DebugConsoleMessage("Failed to allocate memory!\n");
	SHOULD_NEVER_REACH_HERE;
}

void EnableChecking(TDebugInfo<ON>)
{
	/*_CrtSetDbgFlag(
		_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) |
		_CRTDBG_CHECK_ALWAYS_DF |
		_CRTDBG_DELAY_FREE_MEM_DF | _CRTDBG_CHECK_EVERY_128_DF//_CRTDBG_CHECK_EVERY_16_DF _CRTDBG_CHECK_EVERY_1024_DF
		);

	_CrtCheckMemory();*/

	//set_new_handler(no_memory_handler);
}

void EnableChecking(TDebugInfo<OFF>){}

void EnableChecking(void)
{
	EnableChecking(TConfigurationTraits<SCurrentConfig>::DebugInfo());
};

const size_t N=10000;

int aaaa[N*2];

/*typedef struct {                     // interrupt gate descriptor
WORD  Offset_Lower;
WORD  CodeSelector;
unsigned int Id:13;
unsigned int DPL:2;
unsigned int P:1;
unsigned int Offset_Upper:16;
} RTInterruptGate;*/
// typedef void (RTTAPI * RTIntHandler)(void);
/*RTTDLL void        RTTAPI RTSaveVector(BYTE Vector, RTInterruptGate * Gate);
RTTDLL void        RTTAPI RTRestoreVector(BYTE Vector, const RTInterruptGate * Gate);
RTTDLL void        RTTAPI RTSetIntVector(BYTE Vector, RTIntHandler Handler);
RTTDLL void        RTTAPI RTSetTrapVector(BYTE Vector, RTIntHandler Handler);
RTTDLL RTInterruptGate * RTTAPI RTGetIDTR(void);
RTTDLL void        RTTAPI RTSetIDTR(const RTInterruptGate * Table); // must have 128 entries
RTTDLL int         RTTAPI RTCallDebugger(DWORD Command, DWORD P1, DWORD P2);
RTTDLL DWORD       RTTAPI RTCallRing0(RTRing0Function Ring0Func, void * P);
RTTDLL void        RTTAPI RTRaiseCPUException(BYTE Vector);
RTKTaskStartStopHook
*/

	/*template<class T>
	bool IsEmptyItemDefault(const T& Item)
	{
		STATIC_CONST_PLACEMENT_WITHOUT_CALL_ATEXIT(T, DefaultItem);
		return DefaultItem==Item;
	}
	
	template<size_t AmountOfItems, class TItem, TItem& (*getMostSuitable)(TItem&, TItem&), TItem& (*getLeastSuitable)(TItem&, TItem&), bool(*IsEmptyItem)(const TItem&)=IsEmptyItemDefault<TItem> >
	class TFixedSetWithDropLeastSuitable
	{
	public:
		typedef TItem value_type;
		typedef const value_type* const_iterator;
		typedef value_type* iterator;

	private:
		TItem _items[AmountOfItems];

	public:
		TFixedSetWithDropLeastSuitable(void):
			_items()
		{
			STATIC_CHECK((getMostSuitable!=NULL), getMostSuitable_must_is_not_NULL);
			STATIC_CHECK((getLeastSuitable!=NULL), getLeastSuitable_must_is_not_NULL);
			STATIC_CHECK((IsEmptyItem!=NULL), IsEmptyItem_must_is_not_NULL);
		}

		TItem& findMostSuitableItem(void)
		{
			iterator mostSuitableItem=_items;
			for(iterator item=_items, End=EndIteratorOfArray(_items); item!=End; ++item)
			{
				if(IsEmptyItem(*item))
					
			}
			CPL0_ASSERT(mostSuitableItem!=NULL, "Not finded most suitable item");
		}

		TItem& addItemIfMoreSuitable(void)
		{

		}

	}*/
	
	
	
	
	
	class TAreaAllocator
	{
		void* _buffer;
		size_t bufferSize;
		
	public:
		TInternalKernelAreaAllocator(void){}
		void* allocate(void);
		virtual ~TInternalKernelAreaAllocator(void){}
	};

	struct ISimplestAllocator
	{
		void* allocate(void)=0;
		void deallocate(const void* const)=0;
		virtual ~ISimplestAllocator(void){}
	};

	class TInternalKernelPoolAllocator
	{
		//RTAllocPhysPageAligned
		const size_t _SizeOfItem;

	public:
		TInternalKernelPoolAllocator(const size_t SizeOfItem):
			_SizeOfItem(SizeOfItem)
		{}

		void* allocate(void)
		{
		}

		void deallocate(const void* const)
		{
		}

	private:
		
	};

	typedef size_t TInstanceHash;

	template<class TValue>
	TInstanceHash HashOf(const TValue& Value);

	template<class TKey, class TValue>
	class TUnorderedMap
	{
	public:
		TUnorderedMap(ISimplestAllocator& (*)(void) AllocatorGetter, TInstanceHash (*)(const TKey&) HashFunction=::HashOf)
		{

		}
		TValue* find(const TKey& Key)
		
	};

	TInstanceHash HashForUnorderedMap(void* Pointer)
	{
		return (TInstanceHash)Pointer;
	}
};
size_t count_gghh=0;

int OldAccessRights=0;

bool _stdcall f3(void)
{
	return true;
}

bool _stdcall f1(const size_t ErrorCode, const void* const Address)
{
	RTSetPageAccess(Address, RT_PG_USERREADWRITE);
	return (count_gghh++)==0;
}

INT16U flags;

RTIntHandler oldF__=NULL;
RTIntHandler oldF2__=NULL;

__declspec (naked) void f2(void)
{
	_asm{
		cli;
		pushad;
		call f3;
		cmp eax, 0;
		popad;
		jnz IsHandled;
		jmp oldF2__;
IsHandled:
		and word ptr [esp+8], ~0x100;
		iretd;
	}
}


__declspec (naked) void f0(void)
{
	static size_t dummyErrorCode;
	_asm{
		cli;
		pushad;
		mov ebx, esp;
		add ebx, 32;
		pop ebx;
		call RTGetCR2;
		push ebx;
		push eax;
		push [ebx];
		call f1;
		cmp eax, 0;
		popad;
		jnz IsHandled;
		jmp oldF__;
	IsHandled:
		pop dummyErrorCode;
		or word ptr [esp+8], 0x100;
		iretd;
	}
}

RTInterruptGate replaceInterruptVector(const byte InterruptNumber, const RTIntHandler InterruptHandler, RTIntHandler& oldInterruptHandler)
{
	const TDisableInterrupts InterruptDisabler;
	RTInterruptGate oldInterruptGate;
	RTSaveVector(InterruptNumber, &oldInterruptGate);
	oldInterruptHandler=(RTIntHandler)(((size_t)oldInterruptGate.Offset_Upper<<16)+(size_t)oldInterruptGate.Offset_Lower);
	RTSetIntVector(InterruptNumber, InterruptHandler);
	return oldInterruptGate;
}

int __cdecl main (void)
{
	DebugConsoleMessage("Starting System...");

	char aaa[1000];
	std::fill_n(aaa, 999, 0xcd);

	OldAccessRights=RTGetPageAccess(&aaaa[N]);
	RTSetPageAccess(&aaaa[N], RT_PG_USERREAD);

	replaceInterruptVector(14, f0, oldF__);

	replaceInterruptVector(1, f2, oldF2__);

	std::cout<<count_gghh<<std::endl;

	aaaa[N]=0;

	std::cout<<count_gghh<<std::endl;

	DebugConsoleMessage("    InitProtection...");
	InitProtection();

	DebugConsoleMessage("    InitRTK...");
	InitRTK();//Инициализируем ядро системы. Эта стадия необходима здесь для работы всех подсистем.
	DebugConsoleMessage("    ...complete");

	DebugConsoleMessage("    Patching CRT...");
	patchCRT();

	DebugConsoleMessage("    EnableChecking...");
	EnableChecking();

	DebugConsoleMessage("    SetupMonitors...");
	SetupMonitors();//Инициализация супервизоров, следящих за ошибками в системе. Это стадия необходима именно здесь, тк инициализация сетевого оборудования или инициализация передачи файлов уже может произойти с ошибкой и поэтому необходимо "зафиксировать" информацию об этой ошибке в мониторе ошибок, а значит он уже должен существовать.

	DebugConsoleMessage("    Init CNC 2.0 Parts...");
	TNetworkingInitializer::execute();//Инициализация сетевого оборудования. Эта стадия необходима для выполнения стадии инициализации сервера передачи файлов.
	TFileTransferInitializer::execute();//Инициализацию сервера передачи файлов мы выполняем именно здесь, тк это необходимо для транспортировки файлов даже в том случае, если вся СЧПУ "не загрузилась"... Поэтому инициализация этого сервера происходит до общей раскрутки системы.
	TGUISystemInitializer::execute();//Инициализируем "экран", что-бы иметь возможность вывода сообщений об ошибках, даже в том случае, если не загрузилась часть системы. Ну и чем раньше пользователю скажут, мол ждите... идёт загрузка..., тем меньше он будет "паниковать".

	DebugConsoleMessage("    Init com-ports...");
	portManager.initPorts();

	// инициализация системных ресурсов
	DebugConsoleMessage("    Init operation system...");
	TOperationSystem<IS_OS_REAL_TIME>& osObject = TOperationSystem<IS_OS_REAL_TIME>::instance();
	osObject.systemInitialized();

	DebugConsoleMessage("    Starting system tests...");
	TProgressiveTests::runProgressiveTests();
	DebugConsoleMessage("    End of starting system tests.");

	DebugConsoleMessage("    Activate com-ports...");
	portManager.activatePortsInterrupts();
	
	DebugConsoleMessage("    Init system variables...");
	InitSystemVars();

	DebugConsoleMessage("    Create Threads...");
	CreateThreads();

	DebugConsoleMessage("    Start Threads...");
	StartThreads();

	DebugConsoleMessage("...complete");

	for (;;) RTKDelay(1000);
	
	osObject.programmComplete();

	return(0);
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////////