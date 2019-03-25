#pragma once

#include <pplx\pplxinterface.h>

namespace Tools
{
	struct IPplxScheduler : public pplx::scheduler_interface
	{
		virtual ~IPplxScheduler() {};

		virtual void close() = 0;
		virtual void schedule(pplx::TaskProc_t proc, _In_ void* param) override = 0;
	};
}