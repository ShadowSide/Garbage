#ifndef __MEMORY_POOL_H__
#define __MEMORY_POOL_H__

#include "Common\accessory\accessory.h"
#include "ProjectConfiguration.h"
#include "Common\Wrappers\SystemMemoryAllocator.h"
#include "Common\InternalClasses\MemoryAligment.h"
#include "Common\accessory\ExistanceChecker.h"
#include "on_time_rtos\AllocateException.h"
#include "Common\InternalClasses\MemoryPoolInternalClasses.h"

//------------------------------------------------------------------------------------------

COMMENT((

TAbstractMemoryPool - ����������� ��������� ��� ��������� � ����
TMemoryPool - ���������� ����
TMemoryPool ���������� ���������� TAllocateException � ������������ � ������� ��������� ������ allocate.
��������� ������������: ������ ��������, ���������� ���������, ��� ����
allocate - ��������� ������ �� ����
deallocate - ������� ������ � ���
InquiryCount - ���������� ��������� � ����
InquiryDenied - ���������� ����������� ��������� � ����(��������, ��-�� ���������� ��������� �������)
AmountOfFreeItem - ���������� ��������� ��������� � ����
AmountOfItem - ���������� ��������� � ����
ItemsAllocated - ���-�� ��������� ��������� ����
MaxItemsAllocated - ���-�� ����������� ���������� ��������� ����
MaxItemSize - ������ ��������
transferFreeItems - ��������� ���� ��������� ��������� �� ������� ����

)) // end of COMMENT
//-------------------------------------------------------------------------------------------

typedef class TAbstractMemoryPool
{
private:
	enum {MEMORY_POOL_CHECK_SIGNATURE=0xA2d2ef67};
	const TExistanceChecker<MEMORY_POOL_CHECK_SIGNATURE> _PoolExistanceChecker;

public:
	TAbstractMemoryPool(void);

	virtual UNIQUE_POINTED void* allocate(size_t Size)=0;
	virtual void deallocate(const void* const Pointer)=0;
	
	virtual void transferFreeItems(TAbstractMemoryPool& sourcePool)=0;
	virtual ~TAbstractMemoryPool(void){};
	virtual NOTHROW_FUNCTION UNIQUE_POINTED void* allocateWithoutException(size_t Size)=0;
	virtual bool tryFreeAll(void) = 0;
	
//����������:
	virtual unsigned int AmountOfFreeItem(void) const=0;
	virtual unsigned int AmountOfItem(void) const=0;
	virtual size_t MaxItemSize(void) const=0;
	virtual size_t InitialAmountOfItem(void) const=0;
	virtual INT32U InquiryCount(void) const=0;
	virtual INT32U InquiryDenied(void) const=0;
	virtual INT32U ItemsAllocated(void) const=0;
	virtual INT32U MaxItemsAllocated(void) const=0;
	virtual TName Name(void) const=0;
	virtual size_t AlignedItemSize(void) const=0;
	bool empty(void) const;
	void CheckSignatureThatThisIsPool(void) const;
	virtual void checkAllFreeItemsAtPoolIfCheckTurnedOn(void) = 0;
} TAbstractMemoryPoolUsedAtShadowMemoryInformation, *PAbstractMemoryPoolUsedAtShadowMemoryInformation;

//TMemoryAlignmentStrategy - ��������� ������������ ��������� �� ������� � ������� ��� ���������� �������� ������ ����������.
//TMemoryAllocator - �������� ������ ��� ������ ���������.
//��������� TMemoryAllocator ��� TMemoryPoolSimple ������ ����������� ���������� ������������ ����������� ����� � ������������ � TMemoryAlignmentStrategy.
//TMaxPoolCheckStrategy �������� �� ����������� �������� �����:
//TPoolNoCheck - ��� �������� ���� ������, ����� ��������, ���� ��� ��������.
//TPoolCheckMemoryCorruptionOfItemInternalStructures - ����������� ����� �������� ���� ������ ������ � ��� ��������� ��������.
//TPoolDisallowItemFromAnotherPoolAndCheckMemoryCorruptionOfItemInternalStructures - ����������� �������� �������� �� � ��� ���, �� �������� �� ��� ��������. ����� ����� �������� ���� ��-�� ����� ������ � ��� ��������� ��������.
//TMaxPoolCheckStrategy - ��� ���� ����������� ��������� ��������. �� ���� ��� �� ����� ��������� �������� ������������� � ProjectConfiguration.h � ������ TNoneRTOS32PoolsChecking.

//TMemoryPoolSimple �� ���������� ���������� �� � ������������ (�� ������������ �� ��������� TSystemMemoryAllocator), �� � allocate (�� � ��� ������ ���).
//���� �������� TMemoryAllocator �� TSystemMemoryAllocatorCanThrow, �� ����������� ������ ������������ ���������� TAllocateException.
template< class TMemoryAllocator = TSystemMemoryAllocator, class TMaxPoolCheckStrategy=TPoolDisallowItemFromAnotherPoolAndCheckMemoryCorruptionOfItemInternalStructures, class TMemoryAlignmentStrategy=TSystemMemoryAligment >
class TMemoryPoolSimple: public TAbstractMemoryPool
{
private:
	typedef typename TSelectLessDerived<TMaxPoolCheckStrategy, CURRENT_CONFIGURATION_TRAIT(TNoneRTOS32PoolsChecking)::TStatus>::type TPoolCheckStrategy;

public:
	TMemoryPoolSimple(const size_t MaxItemSize, const size_t InitialAmountOfItem, TName Name);
	virtual ~TMemoryPoolSimple(void);

	virtual NOTHROW_FUNCTION UNIQUE_POINTED void* allocate(size_t Size);
	virtual void deallocate(const void* const Pointer);

	virtual void transferFreeItems(TAbstractMemoryPool& sourcePool);

	virtual bool tryFreeAll(void);//�� ������ ��������, ����������� � ��� � ������� transferFreeItems

//����������:
	virtual size_t AmountOfFreeItem(void) const;
	virtual size_t AmountOfItem(void) const;
	virtual size_t InitialAmountOfItem(void) const;
	virtual size_t MaxItemSize(void) const;
	virtual INT32U InquiryCount(void) const;
	virtual INT32U InquiryDenied(void) const;
	virtual INT32U ItemsAllocated(void) const;
	virtual INT32U MaxItemsAllocated(void) const;
	virtual TName Name(void) const;
	virtual void checkAllFreeItemsAtPoolIfCheckTurnedOn(void);

protected:
	virtual size_t AlignedItemSize(void) const;
	NOTHROW_FUNCTION UNIQUE_POINTED void* allocateWithoutException(size_t Size);

private:
	void* _allocationSourceBuffer;
	const size_t _MaxItemSize;
	size_t _allocationSourceBufferSize;
	void* _allocationSourceBufferFirstItem;
	void* _topFreeItem;
		
	typename NMemoryPoolsInternals::TItemMaps<TMemoryAlignmentStrategy>::TFreeItemMap<TPoolCheckStrategy>				_freeItemMap;
	typename NMemoryPoolsInternals::TItemMaps<TMemoryAlignmentStrategy>::TAllocatedItemMap<TPoolCheckStrategy>		_allocatedItemMap;

	const size_t _AlignedItemSize;
	const size_t _PoolBufferSize;

	//����������:
	size_t _amountOfFreeItem;
	size_t _amountOfItem;
	size_t _InitialAmountOfItem;
	size_t _maxItemsAllocated;
	INT32U _inquiryCount;
	INT32U _inquiryDenied;
	TName _Name;

	void CheckItemBelongPoolIfCheckActivated(void* const Base) const;
};

//TMemoryPool ���������� ���������� TAllocateException � ������������ (�� ������������ �� ��������� TSystemMemoryAllocatorCanThrow) � ������� ��������� ������ allocate (�� � ��� ������ ���).
//���� �������� TMemoryAllocator �� TSystemMemoryAllocator, �� ���������� ���������� ������������ ����������.
//�� ���������� ������� ������ TMemoryPoolSimple.
template< class TMemoryAllocator = TSystemMemoryAllocatorCanThrow, class TMaxPoolCheckStrategy=TPoolDisallowItemFromAnotherPoolAndCheckMemoryCorruptionOfItemInternalStructures, class TMemoryAlignmentStrategy=TSystemMemoryAligment >
class TMemoryPool: public TMemoryPoolSimple<TMemoryAllocator, TMaxPoolCheckStrategy, TMemoryAlignmentStrategy>
{
public:
	TMemoryPool(const size_t MaxItemSize, const size_t InitialAmountOfItem, TName Name);
	virtual ~TMemoryPool(void){};
	virtual UNIQUE_POINTED void* allocate(size_t Size);
};


COMMENT((
	��� ������������� ����� ��� TPoolCheckStrategy � TPoolCheckMemoryCorruptionOfItemInternalStructures. �������� TMaxPoolCheckStrategy ���������� � ������������ � ����������� �� ProjectConfiguration.h
	� �� ������ �� �������� ������ �������� � ��������� ������� TPoolCheckMemoryCorruptionOfItemInternalStructures ��� ���� - ������� �������� �������� ����������� �� ��������� ������� � ����� ��������.
	������ TMemoryPool � TMemoryPoolSimple ����� ���������� �������� ����� ������ ��������� (���\���� �������\������) �� ����� �� ���������� � ���� _allocationSourceBufferFirstItem
	������ ���� �������� ���� � 16�������� ���� �������� �� �� �������� �� ��� ������� �� ����
	SIGNATURE_AND_POOL_ITEM_FREE=0xFE (FreE)
	SIGNATURE_AND_POOL_ITEM_ALLOCATED=0xA1 (ALlocated)
	������ ���� ������� �� ������������ ������, ���� �����������
	ALIGNMENT_VOIDS_FILL=0xAF (Alignment_Fill)
	������ ���� ������ � � ������ �� ���������� �������� ���� (� ����� ���� � ������� ������ �� ��������) ��� �������� ���:
	ITEM_VALUE=0x7F (����� ������ ��� ������������ ������ �������������������� ������, ��� ��� �� ������������ ��������� 16�������� �������).
	������ ����� ���� ������� ALIGNMENT_VOIDS_FILL �� ������������ ������, ���� �����������.*/
))


class TAbstractPoolForNew
{
private:
	virtual PAbstractMemoryPoolUsedAtShadowMemoryInformation GetMemoryPool(void) =0;	

public:
	virtual ~TAbstractPoolForNew(void){}
	NOTHROW_FUNCTION UNIQUE_POINTED void* allocateWithoutException(size_t Size);
	UNIQUE_POINTED void* allocate(size_t Size);
	void deallocate(const void* const Pointer);
	size_t ItemSize(void);
	
	static void setShadowMemoryInformation(void* allocatedNowBuffer, TAbstractMemoryPool* owner);
	static void* skipShadowMemoryInformation(void* allocatedNowBuffer);
	static void* getRealAllocatedBuffer(void* buffer);
	static PAbstractMemoryPoolUsedAtShadowMemoryInformation getMemoryPoolFromShadowMemoryInformation(void* buffer);
	static size_t getSizeOfAllocatedBufferWithShadowMemoryInformation(const size_t Size);
	
	typedef PAbstractMemoryPoolUsedAtShadowMemoryInformation* TShadowMemoryInformation;

private:
	template<class TFunctor>
	UNIQUE_POINTED void* allocateUniversal(size_t Size, TFunctor cantAllocateHandler/*for example: void (*)(void)*/)
	{
		ASSERT(Size!=0);
		if(Size==0)
			Size=1;
		ASSERT(Size<=this->ItemSize());
		ASSERT(cantAllocateHandler!=NULL);
		if(Size>this->ItemSize())
		{
			cantAllocateHandler();
			return NULL;
		}
		PAbstractMemoryPoolUsedAtShadowMemoryInformation MemoryPool=this->GetMemoryPool();
		ASSERT(MemoryPool!=NULL);
		void* allocatedBuffer=MemoryPool->allocateWithoutException(Size);
		if(allocatedBuffer==NULL)
		{
			cantAllocateHandler();
			return NULL;
		}
		TAbstractPoolForNew::setShadowMemoryInformation(allocatedBuffer, MemoryPool);//��������� �� ���, �� �������� �������.
		return TAbstractPoolForNew::skipShadowMemoryInformation(allocatedBuffer);
	}
};

template <class TPoolRealization=TMemoryPool<> >
class TPoolForNew: public TAbstractPoolForNew
{
private:
	TPoolRealization _pool;

	PAbstractMemoryPoolUsedAtShadowMemoryInformation GetMemoryPool(void);

public:
	TPoolForNew(const size_t ItemSize, const size_t AmountOfItem, TName Name);
};

template <class TTypeForAllocate, class TPoolRealization=TMemoryPool<> >
class TPoolForNewConcrete: public TPoolForNew<TPoolRealization>
{
public:
	TPoolForNewConcrete(const size_t AmountOfItem, TName Name);
};

#include "Common\Realization\memoryPoolRealization.h"

#endif
