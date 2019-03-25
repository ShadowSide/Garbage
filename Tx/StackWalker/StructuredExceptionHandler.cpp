#include "stdafx.h"

#include "StructuredExceptionHandler.h"
#include <boost/assign/list_of.hpp>
#include <DbgHelp.h>
#include <sstream>
#include <iomanip>

using namespace SEH;

const std::map<DWORD, std::string> StructuredExceptionHelper::ExceptionNameMap = boost::assign::map_list_of
	(EXCEPTION_ACCESS_VIOLATION, "EXCEPTION_ACCESS_VIOLATION")
	(EXCEPTION_ARRAY_BOUNDS_EXCEEDED, "EXCEPTION_ARRAY_BOUNDS_EXCEEDED")
	(EXCEPTION_BREAKPOINT, "EXCEPTION_BREAKPOINT")
	(EXCEPTION_DATATYPE_MISALIGNMENT, "EXCEPTION_DATATYPE_MISALIGNMENT")
	(EXCEPTION_FLT_DENORMAL_OPERAND, "EXCEPTION_FLT_DENORMAL_OPERAND")
	(EXCEPTION_FLT_DIVIDE_BY_ZERO, "EXCEPTION_FLT_DIVIDE_BY_ZERO")
	(EXCEPTION_FLT_INEXACT_RESULT, "EXCEPTION_FLT_INEXACT_RESULT")
	(EXCEPTION_FLT_INVALID_OPERATION, "EXCEPTION_FLT_INVALID_OPERATION")
	(EXCEPTION_FLT_OVERFLOW, "EXCEPTION_FLT_OVERFLOW")
	(EXCEPTION_FLT_STACK_CHECK, "EXCEPTION_FLT_STACK_CHECK")
	(EXCEPTION_FLT_UNDERFLOW, "EXCEPTION_FLT_UNDERFLOW")
	(EXCEPTION_GUARD_PAGE, "EXCEPTION_GUARD_PAGE")
	(EXCEPTION_ILLEGAL_INSTRUCTION, "EXCEPTION_ILLEGAL_INSTRUCTION")
	(EXCEPTION_IN_PAGE_ERROR, "EXCEPTION_IN_PAGE_ERROR")
	(EXCEPTION_INT_DIVIDE_BY_ZERO, "EXCEPTION_INT_DIVIDE_BY_ZERO")
	(EXCEPTION_INT_OVERFLOW, "EXCEPTION_INT_OVERFLOW")
	(EXCEPTION_INVALID_DISPOSITION, "EXCEPTION_INVALID_DISPOSITION")
	(EXCEPTION_INVALID_HANDLE, "EXCEPTION_INVALID_HANDLE")
	(EXCEPTION_NONCONTINUABLE_EXCEPTION, "EXCEPTION_NONCONTINUABLE_EXCEPTION")
	(EXCEPTION_PRIV_INSTRUCTION, "EXCEPTION_PRIV_INSTRUCTION")
	(EXCEPTION_SINGLE_STEP, "EXCEPTION_SINGLE_STEP")
	(EXCEPTION_STACK_OVERFLOW, "EXCEPTION_STACK_OVERFLOW")
	(STATUS_UNWIND_CONSOLIDATE, "STATUS_UNWIND_CONSOLIDATE");

const std::string StructuredExceptionHelper::emptyString;

std::string StructuredExceptionHelper::StackWalk(PEXCEPTION_POINTERS EP, HANDLE hThread /*= ::GetCurrentThread()*/, HANDLE hProcess /*= ::GetCurrentProcess()*/)
{
	::SymSetOptions( /*SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES | SYMOPT_UNDNAME*/ 0);
	if (!::SymInitialize(hProcess, 0, TRUE))
		throw EXCEPTION_CONTINUE_SEARCH;

	STACKFRAME stackFrame = { 0 };

	stackFrame.AddrPC.Mode = AddrModeFlat;
#ifdef _WIN64
	stackFrame.AddrPC.Offset = EP->ContextRecord->Rip;
#else
	stackFrame.AddrPC.Offset = EP->ContextRecord->Eip;
#endif

	stackFrame.AddrFrame.Mode = AddrModeFlat;
#ifdef _WIN64
	stackFrame.AddrFrame.Offset = EP->ContextRecord->Rbp;
#else
	stackFrame.AddrFrame.Offset = EP->ContextRecord->Ebp;
#endif

	stackFrame.AddrStack.Mode = AddrModeFlat;
#ifdef _WIN64
	stackFrame.AddrStack.Offset = EP->ContextRecord->Rsp;
#else
	stackFrame.AddrStack.Offset = EP->ContextRecord->Esp;
#endif

	CONTEXT contextRecord = *EP->ContextRecord;

	DWORD64 symbolDisplacement = 0;
	DWORD lineDisplacement = 0;

	BYTE symbolBuffer[sizeof(SYMBOL_INFO) + sizeof(TCHAR) * MAX_SYM_NAME] = { 0 };

	PSYMBOL_INFO symbol = reinterpret_cast<PSYMBOL_INFO>(&symbolBuffer[0]);
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
	symbol->MaxNameLen = MAX_SYM_NAME;

	BYTE filenameBuffer[sizeof(PCHAR) * MAX_PATH] = { 0 };

	IMAGEHLP_LINE line = { sizeof(line) };
	line.FileName = reinterpret_cast<PCHAR>(&filenameBuffer[0]);


	std::stringstream outputString;

	outputString.imbue(std::locale("C", LC_NUMERIC));

	outputString << std::showbase // show the 0x prefix
		<< std::internal // fill between the prefix and the number
		<< std::setfill('0');

	BOOL res = FALSE;

	while (FALSE != (res = ::StackWalk(
#ifdef _WIN64
		IMAGE_FILE_MACHINE_AMD64,
#else
		IMAGE_FILE_MACHINE_I386,
#endif
		hProcess,
		hThread,
		&stackFrame,
		&contextRecord,
		nullptr,
		::SymFunctionTableAccess,
		::SymGetModuleBase,
		nullptr)))
	{
		if (FALSE == ::SymFromAddr(hProcess, stackFrame.AddrPC.Offset, &symbolDisplacement, symbol) ||
			FALSE == ::SymGetLineFromAddr(hProcess, stackFrame.AddrPC.Offset, &lineDisplacement, &line))
			continue;

		outputString << std::hex << std::setw(sizeof(void *) * 2 + 2) << stackFrame.AddrPC.Offset << std::dec << " " << line.FileName << ":" << line.LineNumber << " " << symbol->Name << std::endl;
	}

	::SymCleanup(hProcess);

	return outputString.str();
}

const std::string & StructuredExceptionHelper::GetExceptionCodeName(DWORD Code)
{
	ExceptionMapType::const_iterator res = ExceptionNameMap.find(Code);
	return ExceptionNameMap.end() != res ? res->second : emptyString;
}



SEHException::SEHException(DWORD Code, const std::string &StackTrace) : std::exception(getExceptionName(Code)), code(Code), stackTrace(StackTrace)
{
}

DWORD SEHException::getExceptionCode() const
{
	return code;
}

const char * SEHException::getStackTrace() const
{
	return stackTrace.c_str();
}

const char * SEHException::getExceptionName(DWORD Code)
{
	return StructuredExceptionHelper::GetExceptionCodeName(Code).c_str();
}



StructuredExceptionHandler::StructuredExceptionHandler() : previousHandler(_set_se_translator(reinterpret_cast<_se_translator_function>(&StructuredExceptionHandler::SETranslatorFunction)))
{
}

StructuredExceptionHandler::~StructuredExceptionHandler()
{
	_set_se_translator(previousHandler);
}

DWORD StructuredExceptionHandler::SETranslatorFunction(DWORD Code, PEXCEPTION_POINTERS EP)
{
	throw SEHException(Code, StructuredExceptionHelper::StackWalk(EP));
}