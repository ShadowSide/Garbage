#include "stdafx.h"
#include "LimitedScheduler.h"
#include <windows.h>
#include <cpprest\asyncrt_utils.h>
#include <winnt.h>
#include <thread>
#include <algorithm>
#include "../Logger.h"
#include <Utils/ApplicationLocale.h>

#pragma comment(lib, "cpprest_xp.lib")
namespace Tools
{
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

	LimitedScheduler::LimitedScheduler(unsigned int limit)
		: _limit(limit)
	{
		static_assert(minThreadCount < maxThreadCount, "LimitedScheduler: minThreadCount < maxThreadCount");
		auto threadCount = static_cast<unsigned int>(ceil(static_cast<double>(min(max(minThreadCount, limit), maxThreadCount)) / maxLevelCount));
		for (unsigned int i = 0; i < maxLevelCount; ++i)
		{
			_limits[i] = threadCount;
			makeThread(i);
		}
	}


	LimitedScheduler::~LimitedScheduler()
	{
		if (!_isClosed)
		{
			try
			{
				close();
			}
			catch (...){}
		}
	}

	void LimitedScheduler::close()
	{
		__AUTO_TRACE_SAFE__(__FUNCSIG__);

		_isCancel = true;
		for (unsigned int i = 0; i < maxLevelCount; ++i)
		{
			_cvLevels[i].notify_all();
		}

		for (unsigned int i = 0; i < maxLevelCount; ++i)
		{
			std::shared_ptr<std::thread> t;
			while (_threads[i].try_pop(t))
			{
				try
				{
					if (t->joinable())
					{
						__AUTO_TRACE_SAFE__(std::string("join to thread ") + boost::lexical_cast<std::string>(t->get_id()));
						t->join();
					}
				}
				catch (...){}
			}
		}

		_isClosed = true;
	}

	void LimitedScheduler::schedule(pplx::TaskProc_t proc, _In_ void* param)
	{
		if (_isClosed)
			throw std::runtime_error("LimitedScheduler is closed");

		auto itLevel = _levelThreads.find(std::this_thread::get_id());
		int level;
		if (itLevel == _levelThreads.end())
			level = 0;
		else
			level = itLevel->second + 1;

		if (level >= maxLevelCount)
			throw std::runtime_error("LimitedScheduler: you can't create the task of " + std::to_string(level) + " level. Max level is " + std::to_string(maxLevelCount) + ".");

		if (_isCancel && level == 0)
			throw std::runtime_error("LimitedScheduler is canceled");
		
		_tasks[level].push({ proc, param });
		addThread(level);
		_cvLevels[level].notify_one();
	}

	void LimitedScheduler::makeThread(unsigned int level)
	{
		std::thread t([this, level]()
		{
			::SetThreadLocale(MAKELCID(
				MAKELANGID(LANG_RUSSIAN, SUBLANG_NEUTRAL), SORT_DEFAULT));
			Tools::setRussianLocale();

			__AUTO_TRACE_SAFE__(std::string("LimitedScheduler thread"));
			try
			{
				auto levelAsString = std::to_string(level);
				auto id = std::this_thread::get_id();
				_stateThreads.insert({ id, ThreadState::free });
				_levelThreads.insert({ id, level });
				while (true)
				{
					if (_isCancel)
						return;

					{
						std::unique_lock<std::mutex> lk(_mtLevels[level]);
						_cvLevels[level].wait(
							lk,
							[this, level](){ return !_tasks[level].empty() || _isCancel; });
					}

					_stateThreads.at(id) = ThreadState::working;
					std::pair<pplx::TaskProc_t, void*> task;
					while (_tasks[level].try_pop(task))
					{
						try
						{
							task.first(task.second);
						}
						catch (...){}
					}
					_stateThreads.at(id) = ThreadState::free;
				}
			}
			catch (...){}
		});
		_threads[level].push(std::make_shared<std::thread>(std::move(t)));
	}

	void LimitedScheduler::addThread(unsigned int level)
	{
		try
		{
			if (_threads[level].unsafe_size() >= _limits[level])
				return;
			if (_tasks[level].unsafe_size() <= 1)
				return;
			auto it = std::find_if(_levelThreads.begin(), _levelThreads.end(), [&](const std::pair<std::thread::id, unsigned int> &val){
				return val.second == level && _stateThreads.at(val.first) == ThreadState::free;
			});
			if (it != _levelThreads.end())
				return;
			std::lock_guard<std::recursive_mutex> guard(_mt[level]);
			if (_threads[level].unsafe_size() >= _limits[level])
				return;
			makeThread(level);
		}
		catch (...){}
	}
}