#ifndef __SYSTEMMEMORYALLOCATOR_H__
#define __SYSTEMMEMORYALLOCATOR_H__

#include "on_time_rtos\TargetWrapper.h"
#include "Common\accessory\Accessory.h"

//��������, ���������� �� ��������� ������ ���������� ���������� ��������� �� ��������� ����.
class TSystemMemoryAllocatorCanThrow//������ ���������� TAllocateException � ������ ���������� ��������� ������.
{
public:
	static UNIQUE_POINTED void* allocate(const size_t Size);
	static NOTHROW_FUNCTION void deallocate(const void* const Pointer);
};

class TSystemMemoryAllocator//���������� NULL � ������ ���������� ��������� ������.
{
public:
	static NOTHROW_FUNCTION UNIQUE_POINTED void* allocate(size_t Size);
	static NOTHROW_FUNCTION void deallocate(const void* const Pointer);
};

class TNewDeleteAllocatorCanThrow//������ ���������� TAllocateException � ������ ���������� ��������� ������.
{
public:
	static UNIQUE_POINTED void* allocate(const size_t Size);
	static NOTHROW_FUNCTION void deallocate(const void* const Pointer);
};

class TNewDeleteAllocator//���������� NULL � ������ ���������� ��������� ������.
{
public:
	static NOTHROW_FUNCTION UNIQUE_POINTED void* allocate(size_t Size);
	static NOTHROW_FUNCTION void deallocate(const void* const Pointer);
};

#endif