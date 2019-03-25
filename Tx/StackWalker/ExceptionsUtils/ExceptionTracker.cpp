#include <stdafx.h>
#include <Utils/ExceptionsUtils/ExceptionTracker.h>
#include <Utils/ExceptionsUtils/ExceptionsLogger.h>
#include <StackWalker.h>
#include <filesystem>
#include <warnings\boost_warnings_disable.h>
#include <boost/scope_exit.hpp>
#include <warnings\boost_warnings_restore.h>
#include "../SCOPE_EXIT.h"

using path = std::tr2::sys::path;
using namespace Exceptions;
using namespace std;

Exceptions::ExceptionsStackWalker::~ExceptionsStackWalker()
{
	_exceptionsLogger = nullptr;
}

Exceptions::ExceptionsStackWalker::ExceptionsStackWalker(ISimpleLogger* exceptionsLogger, MicrosoftSymbolServer usingMode, Heap* heap)
	: _exceptionsLogger(exceptionsLogger)
	, StackWalker(heap, StackWalker::OptionsAll & ~(usingMode == MicrosoftSymbolServer::Use ? 0 : StackWalker::SymUseSymSrv))
{}

void Exceptions::ExceptionsStackWalker::OnOutput(LPCSTR szText)
{
	_exceptionsLogger->logString(szText);
	//StackWalker::OnOutput(szText);
}

void Exceptions::ExceptionsStackWalker::OnAfterLoadModules()
{
	_exceptionsLogger->logStringLine("[Stack trace]:");
}

void Exceptions::ExceptionTracker::initialize(path logPath, MicrosoftSymbolServer usingMode, bool dontFilterCPPException)
{
	if (_me)
		return;
	_dontFilterCPPException = dontFilterCPPException;
	_heap = std::make_unique<WinHeap>();
	_log = std::make_unique<Exceptions::ExceptionsLogger>(logPath);
	_log->logInfo();
	_log->logStringLine(" Logging started.");
	_stackTracer = std::make_unique<ExceptionsStackWalker>(_log.get(), usingMode, _heap.get());
	_stackTracer->Initialize();
	_lastUnhandledExceptionFilter = SetUnhandledExceptionFilter(UnhandledExceptionStackTraceFilter);
	_vectoredExceptionHandlerHandle = AddVectoredExceptionHandler(1, VectoredHandler);
	_me = this;
}

void Exceptions::ExceptionTracker::deinitialize()
{
	if (!_me)
		return;
	SetUnhandledExceptionFilter(_lastUnhandledExceptionFilter);
	RemoveVectoredExceptionHandler(_vectoredExceptionHandlerHandle);
	_me = nullptr;
}

Exceptions::ExceptionTracker::~ExceptionTracker()
{
	deinitialize();
}

void Exceptions::ExceptionTracker::trackerHandler(DWORD exceptionCode, bool isUnhandled)
{ 
	try {
		auto win32LastError = GetLastError();
		SCOPE_EXIT(win32LastError)
		{
			SetLastError(win32LastError);
		};
		auto locker = _stackTracer->locker();
		if(!_stackTracer->lockerIsLocked())
			return;
		++_enterCount;
		SCOPE_EXIT(this)
		{
			if(_enterCount>0)
				--_enterCount;
		};
		if (_enterCount >= 3)
			return;
		if (_enterCount >= 2)
		{
			_log->logError("Exception while stack trace logging!");
			return;
		}
		_log->logInfo();
		_log->logString(" ");
		logSeverity(_log, exceptionCode);
		if (exceptionCode == CPP_EXCEPTION_CODE)
			_log->logString(" C++");
		_log->logString(isUnhandled ? " Unhandled exception code: " : " Exception code: 0x");
		_log->logHexUnsigned(exceptionCode);
		_log->logEndLine();
		logExceptionInfo(_log, exceptionCode);
		_log->logStringLine("Symbols loading...");
		_stackTracer->ShowCallstack();
		_log->logStringLine("End stack trace.");
	}catch (...){}
}

LONG CALLBACK Exceptions::ExceptionTracker::VectoredHandler(_In_ PEXCEPTION_POINTERS ExceptionInfo)
{
	if (!_me)
		return EXCEPTION_CONTINUE_SEARCH;
	switch (ExceptionInfo->ExceptionRecord->ExceptionCode)
	{
	case CPP_EXCEPTION_CODE://Игнорируем исключения C++, чтобы не логировались.
		if (_me->_dontFilterCPPException)
			break;
	//case CLRDBG_NOTIFICATION_EXCEPTION_CODE_:
	//case CLR_EXCEPTION_CODE:
		return EXCEPTION_CONTINUE_SEARCH;
	}
	_me->trackerHandler(ExceptionInfo->ExceptionRecord->ExceptionCode, false);
	return EXCEPTION_CONTINUE_SEARCH;
}

LONG WINAPI Exceptions::ExceptionTracker::UnhandledExceptionStackTraceFilter(_In_ PEXCEPTION_POINTERS ExceptionInfo)
{
	if (!_me)
		return EXCEPTION_CONTINUE_SEARCH;
	_me->trackerHandler(ExceptionInfo->ExceptionRecord->ExceptionCode, true);
	if (_me->_lastUnhandledExceptionFilter)
		return _me->_lastUnhandledExceptionFilter(ExceptionInfo);
	return EXCEPTION_CONTINUE_SEARCH;
}

void Exceptions::ExceptionTracker::logSeverity(std::unique_ptr<ISimpleLogger>& _log, DWORD exceptionCode)
{
	_log->logString("[");
	const char* severity;
	if (exceptionCode >= 0 && exceptionCode <= 0x3FFFFFFF)
		severity = "SUCCESS_EXCEPTION_STATUS";
	else if (exceptionCode >= 0x40000000 && exceptionCode <= 0x7FFFFFFF)
		severity = "INFORMATION_EXCEPTION_STATUS";
	else if (exceptionCode >= 0x80000000 && exceptionCode <= 0xBFFFFFFF)
		severity = "WARNING_EXCEPTION_STATUS";
	else
		severity = "ERROR_EXCEPTION_STATUS";
	_log->logString(severity);
	_log->logString("]");
}

void Exceptions::ExceptionTracker::logExceptionInfo(std::unique_ptr<ISimpleLogger>& _log, DWORD exceptionCode)
{
	if (!m_ntdll.get())
		return;
	const size_t BUF_SIZE = 4096;
	char lpMessageBuffer[BUF_SIZE] = { 0 };
	_log->logString("Probably diagnostic message: ");

	FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_FROM_HMODULE |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		m_ntdll.get(),
		exceptionCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMessageBuffer,
		BUF_SIZE,
		NULL);

	_log->logStringLine(lpMessageBuffer);
}

Exceptions::ExceptionTracker* Exceptions::ExceptionTracker::_me = nullptr;

/*
#include <eh.h>
#include <Windows.h>
extern "C" void __stdcall _CxxThrowException(
void* pExceptionObject,
_ThrowInfo* pThrowInfo
);

typedef struct _DISPATCHER_CONTEXT {
ULONG64 ControlPc;
ULONG64 ImageBase;
PRUNTIME_FUNCTION FunctionEntry;
ULONG64 EstablisherFrame;
ULONG64 TargetIp;
PCONTEXT ContextRecord;
PEXCEPTION_ROUTINE LanguageHandler;
PVOID HandlerData;
} DISPATCHER_CONTEXT, *PDISPATCHER_CONTEXT;

extern "C" _CRTIMP EXCEPTION_DISPOSITION __CxxFrameHandler(
struct EHExceptionRecord  *pExcept,           // Information for this exception
struct EHRegistrationNode *pRN,               // Dynamic information for this frame
CONTEXT            *pContext,          // Context info
PDISPATCHER_CONTEXT pDC                // More dynamic info for this frame
)*/
