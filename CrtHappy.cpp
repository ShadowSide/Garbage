
#include "Common\CrtHappy\CrtHappy.h"

#include <Common\accessory\accessory.h>
#include <stdio.h>
#include <crtdbg.h>
#include <stdarg.h>
#include "Types.h"
#include <string.h>
#include "Common\accessory\clear.h"

enum
{
	MAX_CHARS_AT_CRT_DEBUG_FORMAT_BUFFER_SMALL=800,
	MAX_CHARS_AT_CRT_DEBUG_FORMAT_BUFFER_BIG=MAX_CHARS_AT_CRT_DEBUG_FORMAT_BUFFER_SMALL+600
};


struct TStdAtexit_Ignored{};
struct TStdAtexit_NotPatched{};


struct TCrtDbgReport_NotPatchCRTDbgReport{};

struct TCrtDbgReport_ReportAtConsoleThanASSERTwithFalseForAll
{
	static int afterCallThunk(const int ReportType)
	{
		SHOULD_NEVER_REACH_HERE;
		return -1;//Остановка программы.
	}
};

struct TCrtDbgReport_ReportAtConsoleThanRescueWarningsOrASSERTwithFalseForOther
{
	static int afterCallThunk(const int ReportType)
	{
		switch(ReportType) 
		{
		case _CRT_WARN:
			return 0;//Пытаемся продолжить выполнение.
		case _CRT_ERROR:
		case _CRT_ASSERT:
		default: 
			SHOULD_NEVER_REACH_HERE;
		}
		return -1;//Остановка программы.
	}
};

struct TCrtDbgReport_ReportAtConsoleThanRescueForAll
{
	static int afterCallThunk(const int ReportType)
	{
		return 0;//Пытаемся продолжить выполнение.
	}
	
};

int _cdecl _CrtDbgReportThunk(
			  int reportType,
			  const char *filename,
			  int linenumber,
			  const char *moduleName,
			  const char *format,
			  ... 
			  )
{
	/*#define _CRT_WARN           0
	#define _CRT_ERROR          1
	#define _CRT_ASSERT         2
	#define _CRT_ERRCNT         3  */
	static PChar reportTypes[]=
	{
		"Warning _CrtDbgReport",
		"Error _CrtDbgReport",
		"Assert _CrtDbgReport"
	};
	ASSERT(reportType>=0);
	ASSERT(reportType<_CRT_ERRCNT);
	
	char bufferSmall[MAX_CHARS_AT_CRT_DEBUG_FORMAT_BUFFER_SMALL+1], bufferBig[MAX_CHARS_AT_CRT_DEBUG_FORMAT_BUFFER_BIG+1];
	::clear(bufferSmall);
	::clear(bufferBig);
	
	if(format!=NULL)
	{
		va_list args;
		va_start(args, format);
		const size_t FilledSizeOfBufferSmall = _vsnprintf( bufferSmall, sizeof(bufferSmall) - 1, format, args); // C4996 // Note: vsnprintf is deprecated; consider vsnprintf_s instead
		ASSERT(FilledSizeOfBufferSmall < sizeof(bufferSmall)-1);//Overflow small buffer (see MAX_CHARS_AT_CRT_DEBUG_FORMAT_BUFFER_SMALL).
	};
	const size_t FilledSizeOfBufferBig = _snprintf(bufferBig, sizeof(bufferBig) - 1,
		"%s at file \"%s\" at line \"%u\" at module \"%s\" with message \"%s\"\n",
		reportTypes[reportType], filename, linenumber, moduleName, bufferSmall);
	ASSERT(FilledSizeOfBufferBig < sizeof(bufferBig)-1);//Overflow big buffer (see MAX_CHARS_AT_CRT_DEBUG_FORMAT_BUFFER_BIG).
	printf("%s", bufferBig);

	return CRT_DBG_REPORT_BEHAVIOR::afterCallThunk(reportType);
}

int _cdecl _CrtDbgReportThunkW(
			  int reportType,
			  const wchar_t *filename,
			  int linenumber,
			  const wchar_t *moduleName,
			  const wchar_t *format,
			  ... 
			  )
{
	/*#define _CRT_WARN           0
	#define _CRT_ERROR          1
	#define _CRT_ASSERT         2
	#define _CRT_ERRCNT         3  */
	static PWideChar reportTypes[]=
	{
		L"Warning _CrtDbgReportW",
		L"Error _CrtDbgReportW",
		L"Assert _CrtDbgReportW"
	};
	ASSERT(reportType>=0);
	ASSERT(reportType<_CRT_ERRCNT);
	
	wchar_t bufferSmall[MAX_CHARS_AT_CRT_DEBUG_FORMAT_BUFFER_SMALL+1], bufferBig[MAX_CHARS_AT_CRT_DEBUG_FORMAT_BUFFER_BIG+1];
	::clear(bufferSmall);
	::clear(bufferBig);
	
	if(format!=NULL)
	{
		va_list args;
		va_start(args, format);
		const size_t FilledSizeOfBufferSmall = _vsnwprintf( bufferSmall, sizeof(bufferSmall) - 1, format, args); // C4996 // Note: vsnprintf is deprecated; consider vsnprintf_s instead
		ASSERT(FilledSizeOfBufferSmall < sizeof(bufferSmall)-1);//Overflow small buffer (see MAX_CHARS_AT_CRT_DEBUG_FORMAT_BUFFER_SMALL).
	};
	const size_t FilledSizeOfBufferBig = _snwprintf(bufferBig, sizeof(bufferBig) - 1,
		L"%s at file \"%s\" at line \"%u\" at module \"%s\" with message \"%s\"\n",
		reportTypes[reportType], filename, linenumber, moduleName, bufferSmall);
	ASSERT(FilledSizeOfBufferBig < sizeof(bufferBig)-1);//Overflow big buffer (see MAX_CHARS_AT_CRT_DEBUG_FORMAT_BUFFER_BIG).
	wprintf(L"%s", bufferBig);

	return CRT_DBG_REPORT_BEHAVIOR::afterCallThunk(reportType);
}




void FlushCPUCodeCache(void)//Только для одноядерных машин.
{
	//Пишут, что достаточно даже не длинного перехода для сброса кеша. (Хотя я считал, что нужен длинный).
	//На процессорах старше x486 вроде как и это не нужно.
	//На многоядерных нужно вызывать CPUID.
	_asm
	{
		jmp AfterThisCacheFlushed;//Больше для виду, ввиду того, что обычных переходов в коде должно быть предостаточно.
	AfterThisCacheFlushed:
	}
}

template<class TCrtDbgReportBehavior>
struct TCrtDbgReportPatch
{
	static void patch(void)
	{
		replaceFunctionByMemoryPatch(_CrtDbgReport, _CrtDbgReportThunk);
#if( (_MSC_VER>=1400) || (!defined(_MSC_VER)) ) //for MS VC 2005 и выше.
		replaceFunctionByMemoryPatch(_CrtDbgReportW, _CrtDbgReportThunkW);
#endif
	}
};

template<>
struct TCrtDbgReportPatch<TCrtDbgReport_NotPatchCRTDbgReport>
{
	static void patch(void){}
};



int _cdecl atexit_thunk(void (__cdecl *)( void )){return 0;};

template<class TStdAtexitBehavior>
struct TStdAtexitPatch;

template<>
struct TStdAtexitPatch<TStdAtexit_Ignored>
{
	static void patch(void)
	{
		replaceFunctionByMemoryPatch(atexit, atexit_thunk);
	}
};

template<>
struct TStdAtexitPatch<TStdAtexit_NotPatched>
{
	static void patch(void){}
};



void patchCRT(void)
{
	TCrtDbgReportPatch<CRT_DBG_REPORT_BEHAVIOR>::patch();
	TStdAtexitPatch<FUNCTION_std_atexit_BEHAVIOR>::patch();
}
