#ifndef __SYSTEMMEMORYALLOCATOR_H__
#define __SYSTEMMEMORYALLOCATOR_H__

#include "on_time_rtos\TargetWrapper.h"
#include "Common\accessory\Accessory.h"

//Синглтон, отвечающий за выделение памяти медленными системными функциями из системной кучи.
class TSystemMemoryAllocatorCanThrow//Кидает исключение TAllocateException в случае неудачного выделения памяти.
{
public:
	static UNIQUE_POINTED void* allocate(const size_t Size);
	static NOTHROW_FUNCTION void deallocate(const void* const Pointer);
};

class TSystemMemoryAllocator//Возвращает NULL в случае неудачного выделения памяти.
{
public:
	static NOTHROW_FUNCTION UNIQUE_POINTED void* allocate(size_t Size);
	static NOTHROW_FUNCTION void deallocate(const void* const Pointer);
};

class TNewDeleteAllocatorCanThrow//Кидает исключение TAllocateException в случае неудачного выделения памяти.
{
public:
	static UNIQUE_POINTED void* allocate(const size_t Size);
	static NOTHROW_FUNCTION void deallocate(const void* const Pointer);
};

class TNewDeleteAllocator//Возвращает NULL в случае неудачного выделения памяти.
{
public:
	static NOTHROW_FUNCTION UNIQUE_POINTED void* allocate(size_t Size);
	static NOTHROW_FUNCTION void deallocate(const void* const Pointer);
};

#endif