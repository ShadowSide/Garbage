#pragma once 
#include <exception>

class ExceptionDetector
{
#if((defined(_MSС_VER) && _MSС_VER > 1900) || (!defined(_MSС_VER)))
private:
	int _exceptionCount = 0;
	
public:
	ExceptionDetector()
		: _exceptionCount(std::uncaught_exceptions())
	{}

protected:
	bool uncaught_exception_on_current_level() const
	{
		return _exceptionCount != std::uncaught_exceptions();
	}
#else
protected:
	bool uncaught_exception_on_current_level() const
	{
		return std::uncaught_exception();
	}
#endif

	static bool uncaught_exception_on_any_level()
	{
		return std::uncaught_exception();
	}
	
public:
	ExceptionDetector(const ExceptionDetector&) = delete;
	ExceptionDetector(ExceptionDetector&&) = delete;
	ExceptionDetector& operator=(const ExceptionDetector&) = delete;
	ExceptionDetector& operator=(ExceptionDetector&&) = delete;
};