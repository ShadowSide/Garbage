#pragma once
#include <concurrent_unordered_map.h>
#include <concurrent_queue.h>
#include <condition_variable>
#include <thread>
#include <list>
#include <mutex>
#include "bounded_concurrent_queue.hpp"
#include "IPplxScheduler.h"

namespace Tools
{
	class LimitedScheduler : public IPplxScheduler
	{
	public:
		LimitedScheduler(unsigned int limit = std::thread::hardware_concurrency() * 2);
		~LimitedScheduler();
	public:
		virtual void close() override;
		virtual void schedule(pplx::TaskProc_t proc, _In_ void* param) override;
	private:
		void makeThread(unsigned int level);
		void addThread(unsigned int level);
	private:
		const static unsigned int maxLevelCount = 2;
		const static unsigned int minThreadCount = 2 * maxLevelCount;
		const static unsigned int maxThreadCount = 64;
	private:
		enum ThreadState
		{
			free,
			working
		};
		concurrency::concurrent_unordered_map<std::thread::id, unsigned int > _levelThreads;
		concurrency::concurrent_unordered_map<std::thread::id, ThreadState> _stateThreads;
		concurrency::concurrent_queue<std::shared_ptr<std::thread>> _threads[maxLevelCount];
		std::condition_variable _cvLevels[maxLevelCount];
		std::mutex _mtLevels[maxLevelCount];
		bounded_concurrent_queue<std::pair<pplx::TaskProc_t, void*>> _tasks[maxLevelCount];
		std::recursive_mutex _mt[maxLevelCount];
		unsigned int _limits[maxLevelCount];
		std::atomic<bool> _isCancel = false;
		std::atomic<bool> _isClosed = false;
		unsigned int _limit;
	};

}