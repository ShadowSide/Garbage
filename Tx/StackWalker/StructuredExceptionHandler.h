#pragma once

#include <map>

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <windows.h>

namespace SEH
{
	class StructuredExceptionHelper
	{
	public:
		typedef std::map<DWORD, std::string> ExceptionMapType;
		const static ExceptionMapType ExceptionNameMap;

	public:
		static std::string StackWalk(PEXCEPTION_POINTERS EP, HANDLE hThread = ::GetCurrentThread(), HANDLE hProcess = ::GetCurrentProcess());

		static const std::string & GetExceptionCodeName(DWORD Code);

	private:
		static const std::string emptyString;
	};

	class SEHException : public std::exception
	{
	public:
		SEHException(DWORD Code, const std::string &StackTrace);

		//virtual const char * what() const
		//{
		//	std::stringstream str;
		//	str << std::exception::what() << std::endl << getStackTrace();
		//	return str.str().c_str();
		//}

		virtual DWORD getExceptionCode() const;

		virtual const char * getStackTrace() const;

		static const char * getExceptionName(DWORD Code);

	private:
		DWORD code;
		std::string stackTrace;
	};

	class StructuredExceptionHandler
	{
	public:
		StructuredExceptionHandler();

		~StructuredExceptionHandler();

	protected:
		static DWORD SETranslatorFunction(DWORD Code, PEXCEPTION_POINTERS EP);

	private:
		const _se_translator_function previousHandler;

		const StructuredExceptionHandler & operator=(const StructuredExceptionHandler &);
	};
}