#pragma once
#include "..\..\Singleton.h"
#include "ExceptionsLogger.h"
#include <StackWalker.h>
#include <winbase.h>

namespace Exceptions
{
	enum class MicrosoftSymbolServer
	{
		DontUse,
		Use
	};

	class ExceptionTracker
	{
	public:
		void initialize(std::tr2::sys::path logPath, MicrosoftSymbolServer usingMode = MicrosoftSymbolServer::DontUse, bool dontFilterCPPException = false);
		~ExceptionTracker();
		ExceptionTracker()=default;

	private:
		void deinitialize();
		void trackerHandler(DWORD exceptionCode, bool isUnhandled);
		ExceptionTracker(ExceptionTracker&) = delete;
		ExceptionTracker(ExceptionTracker&&) = delete;
		ExceptionTracker& operator=(ExceptionTracker&) = delete;
		ExceptionTracker& operator=(ExceptionTracker&&) = delete;

		std::unique_ptr<Heap> _heap;
		std::unique_ptr<void, std::function<void(void*)>> m_ntdll{ LoadLibrary("NTDLL.DLL"), [](void* h){if (h)FreeLibrary((HMODULE)h); } };
		std::unique_ptr<ISimpleLogger> _log;
		std::unique_ptr<class ExceptionsStackWalker> _stackTracer;
		static ExceptionTracker* _me;//Не обращайемся к синглтону через синглтон, чтобы потом не поломалось при модификации синглтона, кешируем у себя.
		int _enterCount = 0;
		bool _dontFilterCPPException = false;
		LPTOP_LEVEL_EXCEPTION_FILTER _lastUnhandledExceptionFilter = nullptr;
		PVOID _vectoredExceptionHandlerHandle = nullptr;
		enum 
		{ 
			CPP_EXCEPTION_CODE = 0xE06D7363, 
			CLRDBG_NOTIFICATION_EXCEPTION_CODE_ = 0x4242420,
			RPC_CALL_CANCELED = 0x71a,
			CLR_EXCEPTION_CODE = 0xE0434f4D
		};
		static LONG CALLBACK VectoredHandler(_In_ PEXCEPTION_POINTERS ExceptionInfo);
		static LONG WINAPI UnhandledExceptionStackTraceFilter(_In_ PEXCEPTION_POINTERS ExceptionInfo);
		void logSeverity(std::unique_ptr<ISimpleLogger>& _log, DWORD exceptionCode);
		void logExceptionInfo(std::unique_ptr<ISimpleLogger>& _log, DWORD exceptionCode);
	};

	class ExceptionsStackWalker : public StackWalker
	{
		ISimpleLogger* _exceptionsLogger;
		void OnOutput(LPCSTR szText);
		void OnAfterLoadModules();
	public:
		ExceptionsStackWalker(ISimpleLogger* exceptionsLogger, MicrosoftSymbolServer usingMode, Heap* heap);
		~ExceptionsStackWalker();
	};

	using ExceptionTrackerSngl = Singleton < ExceptionTracker > ;
}