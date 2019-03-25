#pragma once
#include <memory>
#include <functional>
#include <WinNT.h>

struct ISimpleLogger
{
	virtual void logInfo() = 0;
	virtual void logString(const char* const str) = 0;
	virtual void logError(const char* const str) = 0;
	virtual void logEndLine() = 0;
	virtual void logStringLine(const char* const str) = 0;
	virtual void logHexUnsigned(UINT64 value) = 0;
	virtual ~ISimpleLogger();
};

namespace Exceptions
{
	class ExceptionsLogger : public ::ISimpleLogger
	{
	public:
		explicit ExceptionsLogger(std::tr2::sys::path logFile /*= defaultLogPath()*/);
		ExceptionsLogger(ExceptionsLogger&) = delete;
		ExceptionsLogger(ExceptionsLogger&&) = delete;
		ExceptionsLogger& operator=(ExceptionsLogger&) = delete;
		ExceptionsLogger& operator=(ExceptionsLogger&&) = delete;
		void logInfo();
		void logString(const char* const str);
		void logError(const char* const str);
		void logEndLine();
		void logStringLine(const char* const str);
		void logHexUnsigned(UINT64 value);
	private:
		//static std::tr2::sys::path defaultLogPath();
		void write(const char* const data);
		void open(const char* const filename, bool append);
		INT64 filesize(const char* const name) const;
		typedef std::unique_ptr<void, std::function<void(void*)>> Filehandle;
		Filehandle _file;
		const static INT64 truncateAfterMb = 100;
	};
}