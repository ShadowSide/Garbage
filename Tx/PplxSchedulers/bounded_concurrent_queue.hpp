#pragma once
#include <concurrent_queue.h>

namespace Tools
{
	template<typename _Ty, class _Ax = std::allocator<_Ty>>
	class bounded_concurrent_queue : public concurrency::concurrent_queue < _Ty, _Ax >
	{
	public:
		bounded_concurrent_queue(unsigned int max_size = 256)
			:_max_size(max_size)
		{

		}
		void push(const _Ty& _Src)
		{
			wait();
			concurrency::concurrent_queue<_Ty, _Ax>::push(_Src);
		}
		void push(_Ty&& _Src)
		{
			wait();
			concurrency::concurrent_queue<_Ty, _Ax>::push(_Src);
		}
		bool try_pop(_Ty& _Dest)
		{
			auto ret = concurrency::concurrent_queue<_Ty, _Ax>::try_pop(_Dest);
			if (ret)
				_cv.notify_one();
			return ret;
		}
	private:
		void wait()
		{
			if (unsafe_size() >= _max_size)
			{
				_cv.wait(std::unique_lock<std::mutex>(_mt));
			}
		}
	private:
		std::condition_variable _cv;
		std::mutex _mt;
		size_type _max_size;
	};
}