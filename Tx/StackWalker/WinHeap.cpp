#include <stdafx.h>
#include <Utils/WinHeap.h>

WinHeap::WinHeap() :
	_heapHandle(HeapCreate(NULL, 0, 0), [](void* h){if (h)HeapDestroy(h); })
{}

WinHeap::WinHeap(WinHeap&& h) : _heapHandle(std::move(h._heapHandle))
{}

WinHeap::WinHeap(DefaultHeap):
	_heapHandle(GetProcessHeap(), [](void*){})
{}

void WinHeap::Deallocate(void* pointer)
{
	if (!pointer)
		return;
	if (!_heapHandle)
		return;
	HeapFree(_heapHandle.get(), NULL, pointer);
}

void* WinHeap::Allocate(size_t size)
{
	if (size <= 0)
		++size;
	if (!_heapHandle)
		return nullptr;
	return HeapAlloc(_heapHandle.get(), HEAP_ZERO_MEMORY, size);
}

void* WinHeap::Reallocate(void* pointer, size_t size)
{
	throw std::exception("Not implemented");
}

WinHeap& WinHeap::operator=(WinHeap&& h)
{
	_heapHandle = std::move(h._heapHandle);
	return *this;
}

size_t WinHeap::Size(void* pointer)
{
	throw std::exception("Not implemented");
}

char* Heap::cstrdup(const char* str)
{
	if (!str)
		return nullptr;
	size_t len = strlen(str)+1;
	char* buf = (char*)Allocate(len);
	if (!buf)
		return nullptr;
	strcpy_s(buf, len, str);
	return buf;
}

Heap::~Heap()
{

}
