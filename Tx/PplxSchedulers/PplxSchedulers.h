#pragma once
#include "ExecuteNowITT_scheduler.h"
#include <pplx\pplxtasks.h>

namespace
{
	namespace pplx_ext
	{
		// ITT - google it: In this thread.
		pplx::task_options ExecuteNowITT()
		{
			return pplx_ext::ExecuteNowITT_scheduler::instance();
		}

	}
}