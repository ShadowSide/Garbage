#pragma once
#include <pplx\pplxinterface.h>
#include <memory>
namespace {
	namespace pplx_ext
	{
		// ITT - google it: In this thread.
		class ExecuteNowITT_scheduler : public pplx::scheduler_interface
		{
		public:
			virtual void schedule(pplx::TaskProc_t proc, _In_ void* param)
			{
				proc(param);
			}
			static std::shared_ptr<pplx::scheduler_interface> instance()
			{
				return executor;
			}
		private:
			static const std::shared_ptr<pplx::scheduler_interface> executor;
		};

		const std::shared_ptr<pplx::scheduler_interface> ExecuteNowITT_scheduler::executor(std::make_shared<ExecuteNowITT_scheduler>());
	}
}