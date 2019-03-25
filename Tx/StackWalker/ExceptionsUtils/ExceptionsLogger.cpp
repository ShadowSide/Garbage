#include <stdafx.h>
#include <Utils/ExceptionsUtils/ExceptionsLogger.h>
#include <exception>
#include <filesystem>
#include <windows.h>
#include <string.h>
#include <inttypes.h>
using path = std::tr2::sys::path;
namespace Exceptions
{
	ExceptionsLogger::ExceptionsLogger(path logFile):
		_file(INVALID_HANDLE_VALUE, [](void* v){if (v != INVALID_HANDLE_VALUE) FlushFileBuffers(v);  CloseHandle(v); })
	{
		auto fs = filesize(logFile.string().c_str());
		boost::filesystem::create_directories(boost::filesystem::path(logFile.string()).branch_path());
		open(logFile.string().c_str(), fs >= 0 && fs <= truncateAfterMb * 1024 * 1024);
	}

	void ExceptionsLogger::open(const char* const filename, bool append)
	{
		_file.reset(CreateFile(filename, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, append ? OPEN_ALWAYS : CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL));
		if (append && _file.get() != INVALID_HANDLE_VALUE)
			SetFilePointer(_file.get(), 0, NULL, FILE_END);
	}

	void ExceptionsLogger::write(const char* const data)
	{
		if (_file.get() == INVALID_HANDLE_VALUE)
			return;
		DWORD butWhoCares = 0;
		WriteFile(_file.get(), data, strlen(data), &butWhoCares, NULL);
	}

	void ExceptionsLogger::logInfo()
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		DWORD tid = GetCurrentThreadId();
		DWORD pid = GetCurrentProcessId();
		char buf[256] = "";
		sprintf_s(buf, "[%d-%d-%d %d:%d:%d.%d] [P:%d] [T:%d]",
			st.wYear, st.wMonth, st.wDay, st.wHour,
			st.wMinute, st.wSecond, st.wMilliseconds, pid, tid);
		logString(buf);
	}

	void ExceptionsLogger::logString(const char* const str)
	{
		write(str);
	}

	void ExceptionsLogger::logHexUnsigned(UINT64 value)
	{
		char buf[100];
		sprintf_s(buf, "%" PRIx64, value);
		logString(buf);
	}

	void ExceptionsLogger::logEndLine()
	{
		logString("\r\n");
	}

	void ExceptionsLogger::logStringLine(const char* const str)
	{
		logString(str);
		logEndLine();
	}

	void ExceptionsLogger::logError(const char* const str)
	{
		logInfo(); 
		logString(" Tracker error: ");
		logStringLine(str);
	}

	INT64 ExceptionsLogger::filesize(const char* const name) const
	{
		Filehandle hFile = 
		{ 
			CreateFile(name, GENERIC_READ,
				FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL, NULL),
			[](void* v){if (v != INVALID_HANDLE_VALUE) CloseHandle(v); } 
		};
		if (hFile.get() == INVALID_HANDLE_VALUE)
			return -1; // error condition, could call GetLastError to find out more
		LARGE_INTEGER size;
		if (!GetFileSizeEx(hFile.get(), &size))
			return -1; // error condition, could call GetLastError to find out more
		return size.QuadPart;
	}

}

ISimpleLogger::~ISimpleLogger()
{

}
