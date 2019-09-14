#pragma once
#include <functional>
#include <boost/config.hpp>
#include "ExceptionsUtils\ExceptionDetector.h"

namespace scope_exit_detail
{
	namespace {
		class ScopeExit: protected ExceptionDetector
		{
		private:
			using Functor = std::function<void()>;
			Functor _executer;

		public:
			ScopeExit() = delete;
			ScopeExit(const ScopeExit&) = delete;
			ScopeExit(ScopeExit&&) = delete;
			ScopeExit& operator=(const ScopeExit&) = delete;
			ScopeExit& operator=(ScopeExit&&) = delete;

			ScopeExit(Functor executer)
				: _executer(std::move(executer))
			{}

			~ScopeExit() BOOST_NOEXCEPT_IF(false)
			{
				const bool onException = this->uncaught_exception_on_current_level();
				try
				{
					_executer();
				}
				catch(...)
				{
					if(!onException)
						throw;
				}
			}
		};

		class ScopeFailureExit : protected ExceptionDetector
		{
		private:
			using Functor = std::function<void()>;
			Functor _executer;

		public:
			ScopeFailureExit() = delete;
			ScopeFailureExit(const ScopeFailureExit&) = delete;
			ScopeFailureExit(ScopeFailureExit&&) = delete;
			ScopeFailureExit& operator=(const ScopeFailureExit&) = delete;
			ScopeFailureExit& operator=(ScopeFailureExit&&) = delete;

			ScopeFailureExit(Functor executer)
				: _executer(std::move(executer))
			{}

			~ScopeFailureExit() BOOST_NOEXCEPT
			{
				if (this->uncaught_exception_on_current_level())
				try
				{
					_executer();
				}catch(...)
				{
					//ignore
				}
			}
		};

		class ScopeSuccessExit : protected ExceptionDetector
		{
		private:
			using Functor = std::function<void()>;
			Functor _executer;

		public:
			ScopeSuccessExit() = delete;
			ScopeSuccessExit(const ScopeSuccessExit&) = delete;
			ScopeSuccessExit(ScopeSuccessExit&&) = delete;
			ScopeSuccessExit& operator=(const ScopeSuccessExit&) = delete;
			ScopeSuccessExit& operator=(ScopeSuccessExit&&) = delete;

			ScopeSuccessExit(Functor executer)
				: _executer(std::move(executer))
			{}

			~ScopeSuccessExit() BOOST_NOEXCEPT_IF(false)
			{
				if (!this->uncaught_exception_on_current_level())
					_executer();
			}
		};
	}
}
#define TOKEN_CONCAT_INTERNAL(x, y) x ## y
#define TOKEN_CONCAT(x, y) TOKEN_CONCAT_INTERNAL(x, y)
#define SCOPE_EXIT_LAMBDA ::scope_exit_detail::ScopeExit TOKEN_CONCAT(TOKEN_CONCAT(scopeExit,__COUNTER__),__LINE__) = 
#define SCOPE_EXIT(...) SCOPE_EXIT_LAMBDA [__VA_ARGS__]
#define SCOPE_FAILURE_EXIT_LAMBDA ::scope_exit_detail::ScopeFailureExit TOKEN_CONCAT(TOKEN_CONCAT(scopeFailureExit,__COUNTER__),__LINE__) = 
#define SCOPE_FAILURE_EXIT(...) SCOPE_FAILURE_EXIT_LAMBDA [__VA_ARGS__]
#define SCOPE_SUCCESS_EXIT_LAMBDA ::scope_exit_detail::ScopeSuccessExit TOKEN_CONCAT(TOKEN_CONCAT(scopeSuccessExit,__COUNTER__),__LINE__) = 
#define SCOPE_SUCCESS_EXIT(...) SCOPE_SUCCESS_EXIT_LAMBDA [__VA_ARGS__]
