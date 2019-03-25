#pragma once
#include <boost/scope_exit.hpp>
#include <memory>
#include <functional>
#include <winbase.h>
#include <exception>
#include "SCOPE_EXIT.h"
struct Heap
{
	template<class T>
	using Deleter = std::function < void(T*) >;
	template<class T>
	using UniquePtr = std::unique_ptr < T, Deleter<T> > ;
	virtual void* Allocate(size_t size) = 0;
	virtual void Deallocate(void* pointer) = 0;
	virtual void* Reallocate(void* pointer, size_t size) = 0;
	virtual size_t Size(void* pointer) = 0;
	char* cstrdup(const char* str);
	template<class T, class ...Args>
	T* construct(std::nothrow_t, Args&& ... args);
	template<class T>
	void destruct(T* value);
	template<class T, class ...Args>
	UniquePtr<T> construct_unique(std::nothrow_t, Args&& ... args);
	virtual ~Heap();
};

template<class T, class ...Args>
Heap::UniquePtr<T> Heap::construct_unique(std::nothrow_t, Args&& ... args)
{
	return {
		this->construct<T>(std::nothrow, std::forward<Args>(args)...),
		[this](T* v) {this->destruct(v); } 
	};
}

template<class T>
void Heap::destruct(T* value)
{
	typedef int check_incomplete_type[(sizeof(T) > 0)];
	if (!value)
		return;
	SCOPE_EXIT(this, value)
	{
		this->Deallocate(value);
	};
	value->~T();
}

template<class T, class ...Args>
T* Heap::construct(std::nothrow_t, Args&& ... args)
{
	typedef int check_incomplete_type[(sizeof(T) > 0)];
	void* buf_original = Allocate(sizeof(T));
	if (!buf_original)
		return nullptr;
	T* buf = (T*)buf_original;
	SCOPE_FAILURE_EXIT(this, &buf)
	{
		this->Deallocate(buf);
		buf = nullptr;
	};
	::new(buf)T(std::forward<Args>(args)...);
	return buf;
}

const class DefaultHeap {} defaultHeap;

class WinHeap : public Heap
{
public:
	WinHeap();
	explicit WinHeap(DefaultHeap);
	WinHeap(WinHeap&) = delete;
	WinHeap(WinHeap&& h);
	WinHeap& operator=(WinHeap&) = delete;
	WinHeap& operator=(WinHeap&& h);
	void* Allocate(size_t size);
	void Deallocate(void* pointer);
	void* Reallocate(void* pointer, size_t size);
	size_t Size(void* pointer);
	std::unique_ptr<void, std::function<void(void*)>> _heapHandle;
};